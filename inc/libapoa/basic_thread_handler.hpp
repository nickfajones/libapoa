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

#ifndef LIBAPOA_BASIC_THREAD_HANDLER_HPP
#define LIBAPOA_BASIC_THREAD_HANDLER_HPP

#include <libapoa/common.hpp>
#include <libapoa/basic_thread_handler_service.hpp>

#include <asio.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_handler :
    public asio::basic_io_object<basic_thread_handler_service>
  {
  public:
    friend class application_handler_impl;

  public:
    explicit basic_thread_handler(asio::io_service& io_service);
    ~basic_thread_handler();
    
  public:
    void create_thread(thread_callback cb);
    void shutdown_thread(pid_t tid, int retval);

  private:
    void register_main_thread(asio::io_service& process_io_service);
    void start_main_thread(pid_t pid, thread_callback cb);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_HANDLER_HPP
