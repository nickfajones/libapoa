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

#ifndef LIBAPOA_BASIC_THREAD_POOL_HPP
#define LIBAPOA_BASIC_THREAD_POOL_HPP

#include <libapoa/basic_thread_pool_service.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_pool :
    public boost::asio::basic_io_object<basic_thread_pool_service>
  {
  public:
    explicit basic_thread_pool(boost::asio::io_service& io_service);
    ~basic_thread_pool();

  public:
    void create_pool(uint32_t size, boost::system::error_code& ec);
    void destroy_pool();
    boost::asio::io_service& get_thread_service(
      boost::system::error_code& ec);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_POOL_HPP
