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

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>


namespace apoa
{

//#############################################################################
typedef boost::function<
  void (const boost::system::error_code&,
  boost::asio::io_service&)> thread_callback;

}; // namespace apoa

#endif // LIBAPOA_THREAD_HANDLER_TYPES_HPP
