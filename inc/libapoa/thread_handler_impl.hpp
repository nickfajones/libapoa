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

#include <asio.hpp>

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
    void register_main_thread(asio::io_context& process_io_context);
    void start_main_thread(pid_t pid, thread_callback handler);

    void create_thread(
      asio::io_context& parent_io_context,
      thread_callback handler);

    void shutdown_thread(pid_t tid, int retval);

    asio::io_context& get_io_context(pid_t tid);

  private:
    void insert_thread(
      std::shared_ptr<thread_registration> registration);
    void start_thread(
      std::shared_ptr<thread_registration> registration);
    void on_thread_created(
      std::shared_ptr<thread_registration> registration);

    void shutdown_thread1(
      std::shared_ptr<thread_registration> registration);
    void shutdown_thread2(
      std::shared_ptr<thread_registration> registration);

  private:
    std::mutex threads_mutex_;

    std::map<
      pid_t,
      std::shared_ptr<thread_registration> > threads_;
  };

}; // namespace apoa

#endif // LIBAPOA_THREAD_HANDLER_IMPL_HPP
