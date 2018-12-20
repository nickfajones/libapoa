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

#ifndef LIBAPOA_THREAD_HANDLER_TYPES_HPP
#define LIBAPOA_THREAD_HANDLER_TYPES_HPP

#include <system_error>
#include <functional>

#include <asio.hpp>


namespace apoa
{

//#############################################################################
typedef std::function<
  void (const std::error_code&, asio::io_service&)> thread_callback;

}; // namespace apoa

#endif // LIBAPOA_THREAD_HANDLER_TYPES_HPP
