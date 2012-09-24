/*#############################################################################
#
# Copyright (C) 2012 Network Box Corporation Limited
#   Nick Jones <nick.jones@network-box.com>
#   Jeff He <jeff.he@network-box.com>
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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>

#include <libapoa/common.hpp>


namespace apoa
{

//#############################################################################
class thread_handler_impl
  {
  private:
    class thread_registration;
    
  public:
    thread_handler_impl();
    ~thread_handler_impl();

  public:
    void register_main_thread(boost::asio::io_service& process_io_service);
    void start_main_thread(pid_t pid, thread_callback handler);

    void create_thread(
      boost::asio::io_service& parent_io_service,
      thread_callback handler);

    void shutdown_thread(pid_t tid, int retval);
    
    boost::asio::io_service& get_io_service(pid_t tid);

  private:
    void insert_thread(
      boost::shared_ptr<thread_registration> registration);
    void start_thread(
      boost::shared_ptr<thread_registration> registration);
    void on_thread_created(
      boost::shared_ptr<thread_registration> registration);

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
