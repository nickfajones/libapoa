/*#############################################################################
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

#ifndef LIBAPOA_BASIC_THREAD_POOL_IMPL_HPP
#define LIBAPOA_BASIC_THREAD_POOL_IMPL_HPP

#include <libapoa/thread_handler.hpp>

#include <boost/thread.hpp>


namespace apoa
{

class basic_thread_pool_impl;

//#############################################################################
class thread_pool_ref :
    public boost::enable_shared_from_this<thread_pool_ref>
  {
  public:
    thread_pool_ref(boost::asio::io_service& io_service);
    ~thread_pool_ref();

  public:
    void on_thread_start(
      const boost::system::error_code& ec,
      boost::shared_ptr<basic_thread_pool_impl> impl);
    boost::asio::io_service& get_io_service();

  private:
    boost::asio::io_service& m_io_service;
  };



//#############################################################################
class basic_thread_pool_impl :
    public boost::enable_shared_from_this<basic_thread_pool_impl>
  {
  public:
    friend class thread_pool_ref;

  private:
    typedef std::map<pid_t, boost::shared_ptr<thread_pool_ref> >
      threads_map;

  public:
    basic_thread_pool_impl(boost::asio::io_service& io_service);
    ~basic_thread_pool_impl();

  public:
    void create_pool(uint32_t size, boost::system::error_code& ec);
    void destroy_pool();
    boost::asio::io_service& get_thread_service(
      boost::system::error_code& ec);

  private:
    void register_thread(boost::shared_ptr<thread_pool_ref> ref);

  private:
    apoa::thread_handler thread_handler_;

    threads_map pool_;
    uint32_t pool_size_;
    boost::mutex pool_mutex_;
    threads_map::iterator iter_;
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_POOL_IMPL_HPP
