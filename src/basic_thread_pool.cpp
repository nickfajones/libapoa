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
void basic_thread_pool::create_pool(uint32_t size)
  {
  this->service.create_pool(this->implementation, size);
  }

//#############################################################################
void basic_thread_pool::destroy_pool()
  {
  this->service.destroy_pool(this->implementation);
  }

//#############################################################################
boost::asio::io_service& basic_thread_pool::get_thread_service()
  {
  return this->service.get_thread_service(this->implementation);
  }
}; // namespace apoa
