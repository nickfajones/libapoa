/*#############################################################################
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

#ifndef LIBAPOA_THREAD_HANDLER_IMPL_HPP
#define LIBAPOA_THREAD_HANDLER_IMPL_HPP

#include <sys/types.h>

#include <map>

#include <boost/system/error_code.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>


namespace apoa
{

//#############################################################################
class thread_handler_impl
  {
  private:
    class thread_registration;
    
  private:
    typedef boost::function<
      void (const boost::system::error_code&)>
        thread_callback;
    
  public:
    thread_handler_impl();
    ~thread_handler_impl();
    
  public:
    void process_start(
      boost::asio::io_service& process_io_service,
      thread_callback handler);
    
  public:
    void create_thread(
      boost::asio::io_service& parent_io_service,
      thread_callback handler);
    
  public:
    void shutdown_thread(pid_t tid, int retval);
    
  public:
    boost::asio::io_service& get_io_service(pid_t tid);
    int get_process_retval();
    
  private:
    void thread_main(
        boost::shared_ptr<thread_registration> registration);
    
  private:
    void shutdown_thread1(
        boost::shared_ptr<thread_registration> registration);
    void shutdown_thread2(
        boost::shared_ptr<thread_registration> registration);
      
  private:
    boost::mutex threads_mutex_;
    
    std::map<
      pid_t,
      boost::shared_ptr<thread_registration> > threads_;
  };

}; // namespace apoa

#endif // LIBAPOA_THREAD_HANDLER_IMPL_HPP
