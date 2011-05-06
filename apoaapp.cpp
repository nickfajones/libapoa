/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#   nick.jones@network-box.com
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/thread_handler.hpp>
#include <libapoa/signal_handler.hpp>


//#############################################################################
class mythread : public boost::enable_shared_from_this<mythread>
  {
  public:
    explicit mythread(boost::asio::io_service& io_service) :
      m_timer(io_service),
      m_usr1_handler(io_service)
      {
      APOA_PRINT("mythread::mythread");
      }
    
    ~mythread()
      {
      APOA_PRINT("mythread::~mythread");
      
      m_usr1_handler.cancel();
      m_timer.cancel();
      
      apoa::shutdown_thread(0);
      }
    
  public:
    void on_thread_start(
        const boost::system::error_code& ec)
      {
      APOA_PRINT("mythread::on_thread_start");
      
      m_timer.expires_from_now(
        boost::posix_time::seconds(5));
      m_timer.async_wait(
        boost::bind(
          &mythread::on_timeout, this,
          boost::asio::placeholders::error,
          shared_from_this()));
      
      m_usr1_handler.handle(SIGUSR1);
      m_usr1_handler.async_wait(
        boost::bind(
          &mythread::on_sigusr1, this,
          boost::asio::placeholders::error,
          _2));
      }
    
  public:
    void on_timeout(
        const boost::system::error_code& ec,
        boost::shared_ptr<mythread> self)
      {
      if (ec)
        {
        return;
        }
      
      APOA_PRINT("mythread::on_timeout");
      
      m_timer.expires_from_now(
        boost::posix_time::seconds(5));
      m_timer.async_wait(
        boost::bind(
          &mythread::on_timeout, this,
          boost::asio::placeholders::error,
          shared_from_this()));
      }
    
  public:
    void on_sigusr1(
        const boost::system::error_code& ec, apoa::siginfo sigusr1_info)
      {
      APOA_PRINT("mythread::on_sigusr1");
      
      if (ec)
        {
        return;
        }
      
      m_usr1_handler.cancel();
      m_timer.cancel();
      }
    
  private:
    boost::asio::deadline_timer m_timer;
    
    apoa::signal_handler m_usr1_handler;
  };



//#############################################################################
class myapp : public boost::enable_shared_from_this<myapp>
  {
  public:
    explicit myapp(boost::asio::io_service& io_service) :
      m_threads_started(1),
      m_timer(io_service),
      m_thread_handler(io_service),
      m_int_handler(io_service),
      m_usr1_handler(io_service),
      m_usr2_handler(io_service)
      {
      APOA_PRINT("myapp::myapp");
      }
    
    ~myapp()
      {
      APOA_PRINT("myapp::~myapp");
      
      m_usr2_handler.cancel();
      m_usr1_handler.cancel();
      m_int_handler.cancel();
      m_timer.cancel();
      
      apoa::shutdown_process(0);
      }
    
  public:
    void on_application_start(const boost::system::error_code& ec)
      {
      APOA_PRINT("myapp::on_application_start");
      
      m_int_handler.handle(SIGINT);
      m_int_handler.async_wait(
        boost::bind(
          &myapp::on_sigint, this,
          boost::asio::placeholders::error,
          _2));
      
      m_usr1_handler.handle(SIGUSR1);
      m_usr1_handler.async_wait(
        boost::bind(
          &myapp::on_sigusr1, this,
          boost::asio::placeholders::error,
          _2));
      
      m_usr2_handler.handle(SIGUSR2);
      m_usr2_handler.async_wait(
        boost::bind(
          &myapp::on_sigint, this,
          boost::asio::placeholders::error,
          _2));
      
      m_timer.expires_from_now(
        boost::posix_time::seconds(5));
      m_timer.async_wait(
        boost::bind(
          &myapp::on_timeout, this,
          boost::asio::placeholders::error,
          shared_from_this()));
      }
    
  public:
    void on_timeout(
        const boost::system::error_code& ec,
        boost::shared_ptr<myapp> self)
      {
      if (ec)
        {
        return;
        }
      
      APOA_PRINT("myapp::on_timeout");
      
      if (m_threads_started > 0)
        {
        APOA_PRINT("  threads started: '%u'", m_threads_started);
        
        m_thread_handler.create_thread(
          &mythread::on_thread_start, static_cast<mythread*>(NULL));
        
        m_threads_started--;
        }
      
      m_timer.expires_from_now(
        boost::posix_time::seconds(5));
      m_timer.async_wait(
        boost::bind(
          &myapp::on_timeout, this,
          boost::asio::placeholders::error,
          shared_from_this()));
      }
    
  public:
    void on_sigusr1(
        const boost::system::error_code& ec, apoa::siginfo sigusr1_info)
      {
      APOA_PRINT("myapp::on_sigusr1");
      
      if (ec)
        {
        m_usr2_handler.cancel();
        m_usr1_handler.cancel();
        m_int_handler.cancel();
        m_timer.cancel();
        
        return;
        }
      }
    
    void on_sigint(
        const boost::system::error_code& ec, apoa::siginfo sigint_info)
      {
      APOA_PRINT("myapp::on_sigint");
      
      if (ec)
        {
        m_usr2_handler.cancel();
        m_usr1_handler.cancel();
        m_int_handler.cancel();
        m_timer.cancel();
        
        return;
        }
      
      m_usr2_handler.cancel();
      m_usr1_handler.cancel();
      m_int_handler.cancel();
      m_timer.cancel();
      }
    
  private:
    unsigned int m_threads_started;
    
    boost::asio::deadline_timer m_timer;
    
    apoa::thread_handler m_thread_handler;
    
    apoa::signal_handler m_int_handler;
    apoa::signal_handler m_usr1_handler;
    apoa::signal_handler m_usr2_handler;
  };



//#############################################################################
int main(int argc, char** argv)
  {
  boost::asio::io_service io_service;
  apoa::thread_handler process_handler(io_service);
  process_handler.process_init(argc, argv);
  
  return process_handler.process_start(
    &myapp::on_application_start, static_cast<myapp*>(NULL));
  }
