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

#include "common.hpp"
#include "application_handler.hpp"
#include "basic_signal_handler_siginfo.hpp"
#include "basic_signal_handler_service.hpp"
#include "signal_handler_impl.hpp"


namespace apoa
{

//#############################################################################
basic_signal_handler_service::basic_signal_handler_service(
    asio::io_context& io_context) :
  asio::detail::service_base<
    basic_signal_handler_service>(io_context)
  {
  }

basic_signal_handler_service::~basic_signal_handler_service()
  {
  }

//#############################################################################
void basic_signal_handler_service::shutdown_service()
  {
  }

void basic_signal_handler_service::construct(implementation_type& impl)
  {
  impl.reset(new implementation_subtype(get_io_context()));
  }

void basic_signal_handler_service::destroy(implementation_type& impl)
  {
  impl->deactivate();
  impl.reset();
  }

//#############################################################################
std::size_t basic_signal_handler_service::cancel(
    implementation_type& impl,
    std::error_code& ec)
  {
  return impl->cancel(ec);
  }

//#############################################################################
void basic_signal_handler_service::handle(
    implementation_type& impl,
    int signum,
    std::error_code& ec)
  {
  if (is_process_thread())
    {
    impl->handle(signum, ec);

    return;
    }

  apoa::get_process_io_context().post(
    std::bind(
      static_cast<void (signal_handler_base_impl::*)(int)>(
        &signal_handler_base_impl::handle), impl.get(),
      signum));
  }

}; // namespace apoa
