/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#ifndef LIBAPOA_SIGNAL_HANDLER_HPP
#define LIBAPOA_SIGNAL_HANDLER_HPP

#include <libapoa/basic_signal_handler.hpp>
#include <libapoa/basic_signal_handler_service.hpp>
#include <libapoa/signal_handler_impl.hpp>


namespace apoa
{

//#############################################################################
typedef struct basic_siginfo siginfo;
typedef basic_signal_handler<basic_signal_handler_service> signal_handler;

}; // namespace apoa

#endif // LIBAPOA_SIGNAL_HANDLER_HPP
