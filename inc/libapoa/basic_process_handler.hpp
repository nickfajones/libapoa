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

#ifndef LIBAPOA_BASIC_PROCESS_HANDLER_HPP
#define LIBAPOA_BASIC_PROCESS_HANDLER_HPP

#include <string>
#include <list>
#include <map>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>


namespace apoa
{

//#############################################################################
template <typename ProcessService>
class basic_process_handler :
    public boost::asio::basic_io_object<ProcessService>
  {
  public:
    explicit basic_process_handler(boost::asio::io_service& io_service) :
      boost::asio::basic_io_object<ProcessService>(io_service)
      {
      }
    
    virtual ~basic_process_handler()
      {
      }
    
  public:
    void cancel()
      {
      boost::system::error_code ec;
      cancel(ec);
      boost::asio::detail::throw_error(ec);
      }
    
    void cancel(boost::system::error_code& ec)
      {
      this->service.cancel(this->implementation, ec);
      }
    
  public:
    void launch(basic_process_context& context)
      {
      boost::system::error_code ec;
      launch(context, ec);
      boost::asio::detail::throw_error(ec);
      }

    void launch(basic_process_context& context, boost::system::error_code& ec)
      {
      this->service.launch(this->implementation, context, ec);
      }

    void exec(basic_process_context& context, boost::system::error_code& ec)
      {
      this->service.exec(this->implementation, context, ec);
      }
    
    void signal(basic_process_context::signal_type signal_code)
      {
      boost::system::error_code ec;
      this->signal(signal_code, ec);
      boost::asio::detail::throw_error(ec);
      }
    
    void signal(
        basic_process_context::signal_type signal_code,
        boost::system::error_code& ec)
      {
      this->service.signal(this->implementation, signal_code, ec);
      }
    
    template <typename Handler>
    void async_wait_exit(Handler handler)
      {
      this->service.async_wait_exit(this->implementation, handler);
      }
    
  public:
    template <typename MutableBuffer, typename Handler>
    void async_pipe_read(MutableBuffer buffer, Handler handler)
      {
      this->service.async_pipe_read(this->implementation, buffer, handler);
      }
    
    template <typename ConstBuffer, typename Handler>
    void async_pipe_write(ConstBuffer buffer, Handler handler)
      {
      this->service.async_pipe_write(this->implementation, buffer, handler);
      }

  public:
    void cancel_pipe_read()
      {
      this->service.cancel_pipe_read(this->implementation);
      }

    void cancel_pipe_write()
      {
      this->service.cancel_pipe_write(this->implementation);
      }

    void close_pipe_read()
      {
      this->service.close_pipe_read(this->implementation);
      }

    void close_pipe_write()
      {
      this->service.close_pipe_write(this->implementation);
      }

  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_PROCESS_HANDLER_HPP
