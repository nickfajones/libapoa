/*############################################################################
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

#include <libapoa/basic_thread_pool_service.hpp>


namespace apoa
{

//#############################################################################
basic_thread_pool_service::basic_thread_pool_service(
    asio::io_context& io_context) :
  asio::detail::service_base<basic_thread_pool_service>(io_context)
  {
  }

basic_thread_pool_service::~basic_thread_pool_service()
  {
  }

//#############################################################################
void basic_thread_pool_service::shutdown_service()
  {
  }

void basic_thread_pool_service::construct(implementation_type& impl)
  {
  impl.reset(new implementation_type::element_type(get_io_context()));
  }

void basic_thread_pool_service::destroy(implementation_type& impl)
  {
  impl.reset();
  }

//#############################################################################
void basic_thread_pool_service::create_pool(
    implementation_type& impl, uint32_t size, std::error_code& ec)
  {
  impl->create_pool(size, ec);
  }

//#############################################################################
void basic_thread_pool_service::destroy_pool(implementation_type& impl)
  {
  impl->destroy_pool();
  }

//#############################################################################
asio::io_context& basic_thread_pool_service::get_thread_service(
    implementation_type& impl, std::error_code& ec)
  {
  return impl->get_thread_service(ec);
  }
}; // namespace apoa
