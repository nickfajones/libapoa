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

#ifndef LIBAPOA_BASIC_THREAD_POOL_SERVICE_HPP
#define LIBAPOA_BASIC_THREAD_POOL_SERVICE_HPP

#include <memory>
#include <system_error>

#include <libapoa/basic_thread_pool_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_pool_service :
    public asio::detail::service_base<basic_thread_pool_service>
  {
  public:
    typedef std::shared_ptr<basic_thread_pool_impl> implementation_type;
    
  public:
    explicit basic_thread_pool_service(asio::io_service& io_service);
    ~basic_thread_pool_service();

  public:
    virtual void shutdown_service();
    void construct(implementation_type& impl);
    void destroy(implementation_type& impl);

  public:
    void create_pool(implementation_type& impl,
      uint32_t size, std::error_code& ec);
    void destroy_pool(implementation_type& impl);
    asio::io_service& get_thread_service(
      implementation_type& impl, std::error_code& ec);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_POOL_SERVICE_HPP
