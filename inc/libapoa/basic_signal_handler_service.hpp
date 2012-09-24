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

#ifndef LIBAPOA_BASIC_SIGNAL_HANDLER_SERVICE_HPP
#define LIBAPOA_BASIC_SIGNAL_HANDLER_SERVICE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include <libapoa/common.hpp>
#include <libapoa/application_handler.hpp>
#include <libapoa/basic_signal_handler_siginfo.hpp>
#include <libapoa/signal_handler_impl.hpp>


namespace apoa
{

//#############################################################################
class basic_signal_handler_service :
  public boost::asio::detail::service_base<basic_signal_handler_service>
  {
  public:
    typedef boost::shared_ptr<signal_handler_base_impl> implementation_type;
#ifdef __linux__
    typedef signalfd_signal_handler_impl implementation_subtype;
#else
    typedef posix_signal_handler_impl implementation_subtype;
#endif
    
  private:
    typedef boost::function<
      void (const boost::system::error_code&, struct basic_siginfo)>
        basic_signal_callback;
    
  public:
    explicit basic_signal_handler_service(
      boost::asio::io_service& io_service);
    virtual ~basic_signal_handler_service();
    
  public:
    virtual void shutdown_service();
    void construct(implementation_type& impl);
    void destroy(implementation_type& impl);
    
  public:
    std::size_t cancel(
      implementation_type& impl,
      boost::system::error_code& ec);
    
    void handle(
      implementation_type& impl,
      int signum,
      boost::system::error_code& ec);
    
    template <typename NotificationHandler>
    void async_wait(
        implementation_type& impl,
        NotificationHandler handler)
      {
      basic_signal_callback h = handler;
      
      if (is_process_thread())
        {
        impl->async_wait(h);

        return;
        }

      apoa::get_process_io_service().post(
        boost::bind(
          &signal_handler_base_impl::async_wait, impl.get(),
          h));
      }
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_SIGNAL_HANDLER_SERVICE_HPP
