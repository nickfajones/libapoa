/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#ifndef LIBAPOA_PROCESS_HANDLER_HPP
#define LIBAPOA_PROCESS_HANDLER_HPP

#include <libapoa/basic_process_context.hpp>
#include <libapoa/basic_process_handler.hpp>
#include <libapoa/basic_process_handler_service.hpp>


namespace apoa
{

//#############################################################################
typedef basic_process_context process_context;
typedef basic_process_handler<basic_process_handler_service> process_handler;

}; // namespace apoa

#endif // LIBAPOA_PROCESS_HANDLER_HPP
