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

#ifndef LIBAPOA_BASIC_PROCESS_HANDLER_SERVICE_HPP
#define LIBAPOA_BASIC_PROCESS_HANDLER_SERVICE_HPP

#include <memory>
#include <system_error>

#include <asio.hpp>

#include <libapoa/basic_process_context.hpp>
#include <libapoa/process_handler_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_process_handler_service :
    public asio::detail::service_base<basic_process_handler_service>
  {
  public:
    typedef std::shared_ptr<process_handler_impl> implementation_type;
    
  public:
    explicit basic_process_handler_service(
        asio::io_service& io_service);
    virtual ~basic_process_handler_service();
    
  public:
    virtual void shutdown_service();
    void construct(implementation_type& impl);
    void destroy(implementation_type& impl);
    
  public:
    void cancel(
      implementation_type& impl, std::error_code& ec);
    
    void launch(
      implementation_type& impl,
      basic_process_context& context, std::error_code& ec);
    
    void exec(
      implementation_type& impl,
      basic_process_context& context, std::error_code& ec);

    void signal(
      implementation_type& impl,
      basic_process_context::signal_type signal_code,
      std::error_code& ec);

  public:
    template <typename Handler>
    void async_wait_exit(implementation_type& impl, Handler handler)
      {
      impl->async_wait_exit(handler);
      }

    template <typename MutableBuffer, typename Handler>
    void async_pipe_read(implementation_type& impl,
      MutableBuffer buffer, Handler handler)
      {
      impl->async_pipe_read(buffer, handler);
      }

    template <typename ConstBuffer, typename Handler>
    void async_pipe_write(implementation_type& impl,
      ConstBuffer buffer, Handler handler)
      {
      impl->async_pipe_write(buffer, handler);
      }

  public:
    void cancel_pipe_read(implementation_type& impl);
    void cancel_pipe_write(implementation_type& impl);
    void close_pipe_read(implementation_type& impl);
    void close_pipe_write(implementation_type& impl);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_PROCESS_HANDLER_SERVICE_HPP
