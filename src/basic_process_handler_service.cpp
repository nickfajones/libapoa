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

#include <system_error>

#include <asio.hpp>

#include "basic_process_handler_service.hpp"
#include "process_handler_impl.hpp"


namespace apoa
{

//#############################################################################
basic_process_handler_service::basic_process_handler_service(
    asio::io_context& io_context) :
  asio::detail::service_base<
    basic_process_handler_service>(io_context)
  {
  }

basic_process_handler_service::~basic_process_handler_service()
  {
  }

//#############################################################################
void basic_process_handler_service::shutdown_service()
  {
  }

void basic_process_handler_service::construct(implementation_type& impl)
  {
  impl.reset(new implementation_type::element_type(get_io_context()));
  }

void basic_process_handler_service::destroy(implementation_type& impl)
  {
  impl.reset();
  }

//#############################################################################
void basic_process_handler_service::cancel(
    implementation_type& impl,
    std::error_code& ec)
  {
  impl->cancel(ec);
  }

//#############################################################################
void basic_process_handler_service::launch(
    implementation_type& impl,
    basic_process_context& context, std::error_code& ec)
  {
  impl->launch(context, ec);
  }

//#############################################################################
void basic_process_handler_service::exec(
    implementation_type& impl,
    basic_process_context& context, std::error_code& ec)
  {
  impl->exec(context, ec);
  }

//#############################################################################
void basic_process_handler_service::signal(
    implementation_type& impl,
    basic_process_context::signal_type signal_code,
    std::error_code& ec)
  {
  impl->signal(signal_code, ec);
  }

//#############################################################################
void basic_process_handler_service::cancel_pipe_read(
  implementation_type& impl)
  {
  impl->cancel_pipe_read();
  }

void basic_process_handler_service::cancel_pipe_write(
  implementation_type& impl)
  {
  impl->cancel_pipe_write();
  }

void basic_process_handler_service::close_pipe_read(
  implementation_type& impl)
  {
  impl->close_pipe_read();
  }

void basic_process_handler_service::close_pipe_write(
  implementation_type& impl)
  {
  impl->close_pipe_write();
  }

}; // namespace apoa
