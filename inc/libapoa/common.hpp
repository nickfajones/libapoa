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

#ifndef LIBAPOA_COMMON_H
#define LIBAPOA_COMMON_H

#include <sys/types.h>

#include <boost/type_traits.hpp>


namespace apoa
{

//#############################################################################
enum log_level
  {
  log_level_none =                                        0,
  log_level_info =                                        10,
  log_level_debug =                                       100,
  log_level_warning =                                     200,
  log_level_error =                                       500,
  log_level_critical =                                    1000,
  };



//#############################################################################
void set_log_level(log_level level);

void log(log_level level, const char* fmt, ...);



//#############################################################################
#define APOA_PRINT(fmt, ...) \
  apoa::log(apoa::log_level_info, fmt, ##__VA_ARGS__);



//#############################################################################
pid_t get_pid();
pid_t get_tid();
bool is_process_thread();

}; // namespace apoa

#endif // LIBAPOA_COMMON_H
