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

#ifndef LIBAPOA_BASIC_THREAD_HANDLER_SERVICE_HPP
#define LIBAPOA_BASIC_THREAD_HANDLER_SERVICE_HPP

#include <libapoa/thread_handler_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_handler_service :
    public asio::detail::service_base<basic_thread_handler_service>
  {
  public:
    typedef std::shared_ptr<apoa::thread_handler_impl>
      implementation_type;

  public:
    explicit basic_thread_handler_service(
      asio::io_context& io_context);
    virtual ~basic_thread_handler_service();

  public:
    virtual void shutdown_service();
    void construct(implementation_type& impl);
    void destroy(implementation_type& impl);

  public:
    asio::io_context& get_thread_io_context(
      implementation_type& impl, pid_t tid);

    void create_thread(
      implementation_type& impl,
      asio::io_context& parent_io_context,
      thread_callback cb);
    void shutdown_thread(
      implementation_type& impl, pid_t tid, int retval);

    void register_main_thread(
      implementation_type& impl,
      asio::io_context& process_io_context);
    void start_main_thread(
      implementation_type& impl, pid_t pid, thread_callback cb);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_HANDLER_SERVICE_HPP
