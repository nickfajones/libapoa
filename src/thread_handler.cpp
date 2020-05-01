/*#############################################################################
#
# Copyright (C) 2012 Network Box Corporation Limited
#   Nick Jones <nick.jones@network-box.com>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <system_error>
#include <thread>

#include "thread_handler.hpp"


namespace apoa
{

//#############################################################################
per_thread_index<thread_handler::thread_registration>
  thread_handler::thread_registry_(-1);

std::mutex thread_handler::tid_tenum_map_mutex_;
std::map<pid_t, tenum_t> thread_handler::tid_tenum_map_;



//#############################################################################
thread_handler::thread_registration::thread_registration() :
  tid_(0),
  shutdown_countdown_(0),
  retval_(0),
  start_handler_(NULL),
  system_thread_(NULL),
  thread_io_context_(NULL),
  thread_work_(NULL),
  is_process_(false),
  is_shutdown_started_(false),
  is_shutting_down_(false)
  {
  }

thread_handler::thread_registration::~thread_registration()
  {
  thread_work_ = NULL;
  thread_io_context_ = NULL;
  system_thread_ = NULL;
  }



//#############################################################################
thread_handler::thread_handler(asio::io_context& io_context) :
  io_context_(io_context)
  {
  }

thread_handler::~thread_handler()
  {
  }

//#############################################################################
int thread_handler::start_main_thread(thread_callback handler)
  {
  {
  std::lock_guard<std::mutex> pool_lock(tid_tenum_map_mutex_);
  tid_tenum_map_.insert(std::make_pair(get_tid(), get_tenum()));
  }

  priv::per_thread_registry::init();

  thread_registration* registration = new thread_registration();

  registration->tid_ = get_tid();
  registration->shutdown_countdown_ =
    APOA_PROCESS_SHUTDOWN_COUNTDOWN_DEFAULT;
  registration->start_handler_ = handler;
  registration->thread_io_context_ = &io_context_;
  registration->is_process_ = true;

  thread_registry_.do_on_thread_start(get_tenum(), registration);

  run_thread(registration);

  int retval = registration->retval_;

  {
  std::lock_guard<std::mutex> pool_lock(tid_tenum_map_mutex_);
  tid_tenum_map_.erase(registration->tid_);
  }

  thread_registry_.do_on_thread_finish(get_tenum());
  registration = NULL;

  return retval;
  }

void thread_handler::create_thread(thread_callback handler)
  {
  thread_registration* registration = new thread_registration();
  registration->start_handler_ = handler;

  try
    {
    registration->system_thread_ =
      new std::thread(
        std::bind(&on_thread_created, registration));
    }
  catch (std::error_code& ec)
    {
    io_context_.post(
      std::bind(handler, ec, boost::ref(io_context_)));
    }
  }

//#############################################################################
void thread_handler::on_thread_created(thread_registration* registration)
  {
  {
  sigset_t block_sigset;
  sigfillset(&block_sigset);

  pthread_sigmask(SIG_SETMASK, &block_sigset, NULL);
  }

  {
  std::lock_guard<std::mutex> pool_lock(tid_tenum_map_mutex_);
  tid_tenum_map_.insert(std::make_pair(get_tid(), get_tenum()));
  }

  registration->tid_ = get_tid();
  registration->shutdown_countdown_ =
    APOA_CHILD_SHUTDOWN_COUNTDOWN_DEFAULT;
  registration->thread_io_context_ =
    new asio::io_context(1);

  thread_registry_.do_on_thread_start(get_tenum(), registration);

  run_thread(registration);

  // don't clean up registry now, will be done when thread is joined
  }

void thread_handler::run_thread(thread_registration* registration)
  {
  priv::per_thread_registry::thread_start();

  registration->thread_work_ =
    new asio::io_context::work(
      *registration->thread_io_context_);

  registration->thread_io_context_->post(
    std::bind(
      registration->start_handler_,
        std::error_code(),
        boost::ref(*registration->thread_io_context_)));

  registration->start_handler_ = NULL;

  registration->thread_io_context_->run();

  delete registration->thread_work_;
  registration->thread_work_ = NULL;

  if (!registration->is_process_)
    {
    delete registration->thread_io_context_;
    registration->thread_io_context_ = NULL;

    thread_registration& process_registration =
      thread_registry_.get(0);

    process_registration.thread_io_context_->post(
      std::bind(&join_thread, get_tenum()));
    }

  priv::per_thread_registry::thread_finish();
  }

//#############################################################################
void thread_handler::shutdown_thread_tid(pid_t tid, int retval)
  {
  tenum_t tenum(0);

  {
  std::lock_guard<std::mutex> tid_tenum_map_lock(tid_tenum_map_mutex_);
  tenum = tid_tenum_map_.find(tid)->second;
  }

  return shutdown_thread(tenum, retval);
  }

void thread_handler::shutdown_thread(tenum_t tenum, int retval)
  {
  thread_registration& registration =
    thread_registry_.get(tenum);

  if (tenum != get_tenum())
    {
    registration.thread_io_context_->post(
      std::bind(&shutdown_thread, tenum, retval));

    return;
    }

  if (registration.is_shutdown_started_)
    {
    return;
    }

  registration.retval_ = retval;
  registration.is_shutdown_started_ = true;

  if (!registration.is_process_)
    {
    // Child thread
    shutdown_thread_initial(tenum);

    return;
    }

  std::lock_guard<std::mutex> callback_lock(
    priv::per_thread_registry::callback_mutex_);

  // Main thread
  if (priv::per_thread_registry::thread_set_.size() == 1)
    {
    // No child threads
    shutdown_thread_initial(0);

    return;
    }

  for (std::set<tenum_t>::iterator itr =
         priv::per_thread_registry::thread_set_.begin();
       itr != priv::per_thread_registry::thread_set_.end();
       ++itr)
    {
    thread_registration& child_registration =
      thread_registry_.get(*itr);

    if (child_registration.is_process_ ||
        child_registration.is_shutdown_started_)
      {
      continue;
      }

    child_registration.retval_ = retval;
    child_registration.is_shutdown_started_ = true;

    child_registration.thread_io_context_->post(
      std::bind(&shutdown_thread_initial, *itr));
    }
  }

//#############################################################################
void thread_handler::shutdown_thread_initial(apoa::tenum_t tenum)
  {
  thread_registration& registration =
    thread_registry_.get(tenum);

  registration.is_shutting_down_ = true;

  if (--registration.shutdown_countdown_ > 0)
    {
    registration.thread_io_context_->post(
      std::bind(&shutdown_thread_initial, tenum));

    return;
    }

  registration.thread_io_context_->stop();
  }

void thread_handler::join_thread(apoa::tenum_t tenum)
  {
  {
  thread_registration& registration =
    thread_registry_.get(tenum);

  registration.system_thread_->join();
  delete registration.system_thread_;
  registration.system_thread_ = NULL;

  {
  std::lock_guard<std::mutex> pool_lock(tid_tenum_map_mutex_);
  tid_tenum_map_.erase(registration.tid_);
  }
  }

  thread_registry_.do_on_thread_finish(tenum);

  thread_registration& process_registration =
    thread_registry_.get(0);

  std::lock_guard<std::mutex> callback_lock(
    priv::per_thread_registry::callback_mutex_);

  if (process_registration.is_shutdown_started_ &&
      (priv::per_thread_registry::thread_set_.size() == 1) &&
      !process_registration.is_shutting_down_)
    {
    shutdown_thread_initial(0);
    }
  }

//#############################################################################
asio::io_context& thread_handler::get_io_context_tid(pid_t tid)
  {
  tenum_t tenum(0);

  {
  std::lock_guard<std::mutex> tid_tenum_map_lock(tid_tenum_map_mutex_);
  tenum = tid_tenum_map_.find(tid)->second;
  }

  return get_io_context(tenum);
  }

asio::io_context& thread_handler::get_io_context(tenum_t tenum)
  {
  return *thread_registry_.get(tenum).thread_io_context_;
  }
}; // namespace apoa
