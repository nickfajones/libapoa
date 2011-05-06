/*############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#   nick.jones@network-box.com
#   jeff.he@network-box.com
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <sys/types.h>
#include <signal.h>

#include <map>

#include <boost/noncopyable.hpp>
#include <boost/system/error_code.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/thread_handler_impl.hpp>


namespace apoa
{

//#############################################################################
static const unsigned int thread_process_shutdown_countdown_default = 30;
static const unsigned int thread_child_shutdown_countdown_default = 10;

static int thread_process_retval_ = 0;



//#############################################################################
class thread_handler_impl::thread_registration : public boost::noncopyable
  {
  public:
    thread_registration() :
      tid_(0), 
      is_process_(false),
      is_shutdown_started_(false),
      is_shutting_down_(false),
      shutdown_countdown_(0),
      retval_(0),
      thread_work_(NULL),
      thread_io_service_(NULL),
      system_thread_(NULL)
      {
      }
    
    ~thread_registration()
      {
      system_thread_ = NULL;
      thread_io_service_ = NULL;
      thread_work_ = NULL;
      }
    
  public:
    pid_t tid_;
    
    bool is_process_;
    bool is_shutdown_started_;
    bool is_shutting_down_;
    unsigned int shutdown_countdown_;
    int retval_;
    
    boost::asio::io_service::work* thread_work_;
    
    boost::asio::io_service* thread_io_service_;
    boost::thread* system_thread_;
    
    boost::function<void (boost::system::error_code)> start_handler_;
  };



//#############################################################################
thread_handler_impl::thread_handler_impl()
  {
  }

thread_handler_impl::~thread_handler_impl()
  {
  }

//#############################################################################
void thread_handler_impl::process_start(
    boost::asio::io_service& process_io_service,
    thread_callback handler)
  {
  boost::shared_ptr<thread_registration> registration(
    new thread_registration());
  registration->tid_ = getpid();
  registration->is_process_ = true;
  registration->shutdown_countdown_ =
    thread_process_shutdown_countdown_default;
  registration->thread_io_service_ = &process_io_service;
  registration->start_handler_ = handler;
  
  thread_main(registration);
  
  thread_process_retval_ = registration->retval_;
  }

//#############################################################################
void thread_handler_impl::create_thread(
    boost::asio::io_service& parent_io_service,
    thread_callback handler)
  {
  boost::shared_ptr<thread_registration> registration(
    new thread_registration());
  registration->shutdown_countdown_ =
    thread_child_shutdown_countdown_default;
  registration->start_handler_ = handler;
  
  try
    {
    registration->system_thread_ =
      new boost::thread(
        boost::bind(
          &thread_handler_impl::thread_main, this,
          registration));
    }
  catch (boost::thread_resource_error& err)
    {
    boost::system::error_code ec;
    
    parent_io_service.post(boost::bind(handler, ec));
    }
  }

//#############################################################################
void thread_handler_impl::shutdown_thread(pid_t tid, int retval)
  {
  boost::unique_lock<boost::mutex> threads_lock(threads_mutex_);
  std::map<pid_t, boost::shared_ptr<thread_registration> >::iterator iter;
  boost::shared_ptr<thread_registration> registration;
  
  do
    {
    // If thread id registered
    iter = threads_.find(tid);
    if (iter == threads_.end())
      {
      break;
      }

    registration = iter->second;

    // Child thread
    if (!registration->is_process_)
      {
      if (registration->is_shutting_down_)
        {
        break;
        }

      registration->is_shutting_down_ = true;
      registration->retval_ = retval;

      shutdown_thread1(registration);

      break;
      }

    // Main thread
    if (registration->is_shutdown_started_)
      {
      break;
      }

    // No child thread
    if (threads_.size() == 1)
      {
      registration->is_shutdown_started_ = true;
      registration->is_shutting_down_ = true;
      registration->retval_ = retval;
      shutdown_thread1(registration);

      break;
      }

    for (iter = threads_.begin(); iter != threads_.end(); iter++)
      {
      boost::shared_ptr<thread_registration> child_registration =
          iter->second;

      if (child_registration->is_process_ ||
          child_registration->is_shutting_down_)
        {
        continue;
        }

      child_registration->is_shutting_down_ = true;
      child_registration->retval_ = retval;

      shutdown_thread1(child_registration);
      }

    registration->is_shutdown_started_ = true;
    registration->retval_ = retval;
    } while (false);
  }

//#############################################################################
boost::asio::io_service& thread_handler_impl::get_io_service(pid_t tid)
  {
  boost::unique_lock<boost::mutex> threads_lock(threads_mutex_);
  
  return *threads_[tid]->thread_io_service_;
  }
    
int thread_handler_impl::get_process_retval()
  {
  return thread_process_retval_;
  }

//#############################################################################
void thread_handler_impl::thread_main(
    boost::shared_ptr<thread_registration> registration)
  {
  if (!registration->is_process_)
    {
    registration->tid_ = get_tid();
    registration->thread_io_service_ =
      new boost::asio::io_service();
      
    sigset_t block_sigset;
    sigfillset(&block_sigset);
    
    pthread_sigmask(SIG_SETMASK, &block_sigset, NULL);
    }
  
  registration->thread_work_ =
    new boost::asio::io_service::work(
      *registration->thread_io_service_);
  
    {
    boost::unique_lock<boost::mutex> threads_lock(threads_mutex_);
    
    threads_.insert(
      std::pair<pid_t, boost::shared_ptr<thread_registration> >(
        registration->tid_, registration));
    }

    {
    boost::system::error_code ec;
    
    registration->thread_io_service_->post(
      boost::bind(registration->start_handler_, ec));
    registration->start_handler_ = NULL;
    }

  registration->thread_io_service_->run();

  delete registration->thread_work_;
  registration->thread_work_ = NULL;

  if (!registration->is_process_)
    {
    delete registration->thread_io_service_;
    registration->thread_io_service_ = NULL;
    
    boost::unique_lock<boost::mutex> threads_lock(threads_mutex_);

    threads_[getpid()]->thread_io_service_->post(
    boost::bind(
      &thread_handler_impl::shutdown_thread2, this,
      registration));
    }

  boost::unique_lock<boost::mutex> threads_lock(threads_mutex_);

  threads_.erase(registration->tid_);
  }

//#############################################################################
void thread_handler_impl::shutdown_thread1(
    boost::shared_ptr<thread_registration> registration)
  {
  if (--registration->shutdown_countdown_ > 0)
    {
    registration->thread_io_service_->post(
      boost::bind(
        &thread_handler_impl::shutdown_thread1, this,
        registration));
    
    return;
    }
  
  registration->thread_io_service_->stop();
  }

void thread_handler_impl::shutdown_thread2(
    boost::shared_ptr<thread_registration> registration)
  {
  registration->system_thread_->join();
  delete registration->system_thread_;
  registration->system_thread_ = NULL;

  boost::unique_lock<boost::mutex> threads_lock(threads_mutex_);

  boost::shared_ptr<thread_registration> process_registration =
    threads_[getpid()];

  if (process_registration->is_shutdown_started_ &&
      (threads_.size() == 1) &&
      !process_registration->is_shutting_down_)
    {
    process_registration->is_shutting_down_ = true;

    shutdown_thread1(process_registration);
    }
  }

}; // namespace apoa
