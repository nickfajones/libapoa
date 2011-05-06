/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#   nick.jones@network-box.com
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#ifndef LIBAPOA_BASIC_THREAD_HANDLER_SERVICE_HPP
#define LIBAPOA_BASIC_THREAD_HANDLER_SERVICE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <libapoa/thread_handler_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_thread_handler_service :
    public boost::asio::detail::service_base<basic_thread_handler_service>
  {
  public:
    typedef boost::shared_ptr<apoa::thread_handler_impl>
      implementation_type;
    
  public:
    explicit basic_thread_handler_service(
      boost::asio::io_service& io_service);
    virtual ~basic_thread_handler_service();
    
  public:
    virtual void shutdown_service();
    void construct(implementation_type& impl);
    void destroy(implementation_type& impl);
    
  public:
    template <typename ProcessStartHandler>
    void process_start(
        implementation_type& impl,
        boost::asio::io_service& process_io_service,
        ProcessStartHandler handler)
      {
      impl->process_start(process_io_service, handler);
      }
    
  public:
    template <typename CreateThreadHandler>
    void create_thread(
        implementation_type& impl,
        boost::asio::io_service& parent_io_service,
        CreateThreadHandler handler)
      {
      impl->create_thread(
        parent_io_service, handler);
      }
    
    /*
    template <typename ShutdownThreadHandler>
    void async_shutdown_notify(
        implementation_type& impl,
        ShutdownThreadHandler handler)
      {
      impl->shutdown_notify(io_service_, tid, handler);
      }
      */
    
  public:
    boost::asio::io_service& get_thread_io_service(
      implementation_type& impl, pid_t tid);
    
  public:
    void shutdown_thread(
      implementation_type& impl, pid_t tid, int retval);
    
  public:
    int get_process_retval(implementation_type& impl);
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_THREAD_HANDLER_SERVICE_HPP
