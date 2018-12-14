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


#include <libapoa/basic_thread_pool.hpp>


namespace apoa
{

//#############################################################################
basic_thread_pool::basic_thread_pool(boost::asio::io_service& io_service) :
  boost::asio::basic_io_object<basic_thread_pool_service>(io_service)
  {
  }

basic_thread_pool::~basic_thread_pool()
  {
  }

//#############################################################################
void basic_thread_pool::create_pool(
    uint32_t size, boost::system::error_code& ec)
  {
  this->get_service().create_pool(this->get_implementation(), size, ec);
  }

//#############################################################################
void basic_thread_pool::destroy_pool()
  {
  this->get_service().destroy_pool(this->get_implementation());
  }

//#############################################################################
boost::asio::io_service& basic_thread_pool::get_thread_service(
    boost::system::error_code& ec)
  {
  return this->get_service().get_thread_service(this->get_implementation(), ec);
  }
}; // namespace apoa
