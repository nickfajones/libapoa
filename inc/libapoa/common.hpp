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

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>
#include <boost/type_traits.hpp>
#include <boost/system/error_code.hpp>


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



//#############################################################################
typedef boost::function<void (const boost::system::error_code&)>
  application_callback;

boost::asio::io_service& get_process_io_service();
void shutdown_process(int retval = 0);

bool process_has_option(const std::string& option_name);
boost::program_options::variable_value get_process_option(
  const std::string& option_name);

boost::asio::io_service& get_io_service(pid_t tid = 0);
void shutdown_thread(pid_t tid = apoa::get_tid(), int retval = 0);

}; // namespace apoa

#endif // LIBAPOA_COMMON_H
