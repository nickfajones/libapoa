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

#ifndef LIBAPOA_BASIC_THREAD_POOL_IMPL_HPP
#define LIBAPOA_BASIC_THREAD_POOL_IMPL_HPP

#include <libapoa/thread_handler.hpp>

#include <boost/thread.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_pool_impl :
    public boost::enable_shared_from_this<basic_thread_pool_impl>
  {
  private:
    typedef std::map<pid_t, boost::asio::io_service&> threads_map;

  public:
    basic_thread_pool_impl(boost::asio::io_service& io_service);
    ~basic_thread_pool_impl();

  public:
    void create_pool(uint32_t size, boost::system::error_code& ec);
    void destroy_pool();
    boost::asio::io_service& get_thread_service(
      boost::system::error_code& ec);

  private:
    void on_thread_created(
      const boost::system::error_code& ec,
      boost::asio::io_service& io_service);

  private:
    thread_handler thread_handler_;

  private:
    boost::mutex pool_mutex_;

    threads_map pool_;
    threads_map::iterator iter_;

  private:
    uint32_t pool_size_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_POOL_IMPL_HPP
