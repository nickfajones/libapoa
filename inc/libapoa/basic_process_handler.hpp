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

#ifndef LIBAPOA_BASIC_PROCESS_HANDLER_HPP
#define LIBAPOA_BASIC_PROCESS_HANDLER_HPP

#include <string>
#include <list>
#include <map>
#include <system_error>

#include <asio.hpp>


namespace apoa
{

//#############################################################################
template <typename ProcessService>
class basic_process_handler :
    public asio::basic_io_object<ProcessService>
  {
  public:
    explicit basic_process_handler(asio::io_service& io_service) :
      asio::basic_io_object<ProcessService>(io_service)
      {
      }

    ~basic_process_handler()
      {
      }

  public:
    void cancel()
      {
      std::error_code ec;
      cancel(ec);
      asio::detail::throw_error(ec);
      }

    void cancel(std::error_code& ec)
      {
      this->get_service().cancel(this->get_implementation(), ec);
      }

  public:
    void launch(basic_process_context& context)
      {
      std::error_code ec;
      launch(context, ec);
      asio::detail::throw_error(ec);
      }

    void launch(basic_process_context& context, std::error_code& ec)
      {
      this->get_service().launch(this->get_implementation(), context, ec);
      }

    void exec(basic_process_context& context, std::error_code& ec)
      {
      this->get_service().exec(this->get_implementation(), context, ec);
      }

    void signal(basic_process_context::signal_type signal_code)
      {
      std::error_code ec;
      this->signal(signal_code, ec);
      asio::detail::throw_error(ec);
      }

    void signal(
        basic_process_context::signal_type signal_code,
        std::error_code& ec)
      {
      this->get_service().signal(this->get_implementation(), signal_code, ec);
      }

    template <typename Handler>
    void async_wait_exit(Handler handler)
      {
      this->get_service().async_wait_exit(this->get_implementation(), handler);
      }

  public:
    template <typename MutableBuffer, typename Handler>
    void async_pipe_read(MutableBuffer buffer, Handler handler)
      {
      this->get_service().async_pipe_read(
        this->get_implementation(), buffer, handler);
      }

    template <typename ConstBuffer, typename Handler>
    void async_pipe_write(ConstBuffer buffer, Handler handler)
      {
      this->get_service().async_pipe_write(
        this->get_implementation(), buffer, handler);
      }

  public:
    void cancel_pipe_read()
      {
      this->get_service().cancel_pipe_read(this->get_implementation());
      }

    void cancel_pipe_write()
      {
      this->get_service().cancel_pipe_write(this->get_implementation());
      }

    void close_pipe_read()
      {
      this->get_service().close_pipe_read(this->get_implementation());
      }

    void close_pipe_write()
      {
      this->get_service().close_pipe_write(this->get_implementation());
      }

  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_PROCESS_HANDLER_HPP
