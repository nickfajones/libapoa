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

#ifndef LIBAPOA_BASIC_SIGNAL_HANDLER_HPP
#define LIBAPOA_BASIC_SIGNAL_HANDLER_HPP

#include <system_error>

#include <asio.hpp>

#include <libapoa/basic_signal_handler_siginfo.hpp>


namespace apoa
{

//#############################################################################
template <typename SignalService>
class basic_signal_handler :
    public asio::basic_io_object<SignalService>
  {
  public:
    explicit basic_signal_handler(asio::io_context& io_context) :
      asio::basic_io_object<SignalService>(io_context)
      {
      }

    ~basic_signal_handler()
      {
      }

  public:
    std::size_t cancel()
      {
      std::error_code ec;
      std::size_t s = cancel(ec);
      asio::detail::throw_error(ec);
      return s;
      }

    std::size_t cancel(std::error_code& ec)
      {
      return this->get_service().cancel(this->get_implementation(), ec);
      }

  public:
    void handle(int signum)
      {
      std::error_code ec;
      handle(signum, ec);
      asio::detail::throw_error(ec);
      }

    void handle(int signum, std::error_code& ec)
      {
      this->get_service().handle(this->get_implementation(), signum, ec);
      }

  public:
    template <typename NotificationHandler>
    void async_wait(NotificationHandler handler)
      {
      this->get_service().async_wait(this->get_implementation(), handler);
      }
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_SIGNAL_HANDLER_SERVICE_HPP
