/*############################################################################
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


#include <libapoa/basic_thread_pool_service.hpp>


namespace apoa
{

//#############################################################################
basic_thread_pool_service::basic_thread_pool_service(
    boost::asio::io_service& io_service) :
  boost::asio::detail::service_base<basic_thread_pool_service>(io_service)
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
  impl.reset(new implementation_type::value_type(get_io_service()));
  }

void basic_thread_pool_service::destroy(implementation_type& impl)
  {
  impl.reset();
  }

//#############################################################################
void basic_thread_pool_service::create_pool(
    implementation_type& impl, uint32_t size)
  {
  impl->create_pool(size);
  }

//#############################################################################
void basic_thread_pool_service::destroy_pool(implementation_type& impl)
  {
  impl->destroy_pool();
  }

//#############################################################################
boost::asio::io_service& basic_thread_pool_service::get_thread_service(
    implementation_type& impl)
  {
  return impl->get_thread_service();
  }
}; // namespace apoa
