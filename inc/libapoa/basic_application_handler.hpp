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

#ifndef LIBAPOA_BASIC_APPLICATION_HANDLER_HPP
#define LIBAPOA_BASIC_APPLICATION_HANDLER_HPP

#include <sys/types.h>

#include <boost/bind.hpp>

#include <libapoa/common.hpp>
#include <libapoa/thread_handler_types.hpp>
#include <libapoa/basic_application_handler_service.hpp>


namespace apoa
{

//#############################################################################
class basic_application_handler :
    public boost::asio::basic_io_object<basic_application_handler_service>
  {
  public:
    explicit basic_application_handler(
        boost::asio::io_service& io_service) :
      boost::asio::basic_io_object<basic_application_handler_service>(io_service)
      {
      }
    ~basic_application_handler()
      {
      }

  public:
    void process_init(
        boost::program_options::options_description& options_desc,
        int argc, char** argv)
      {
      this->service.process_init(this->implementation, options_desc, argc, argv);
      }

    bool process_has_option(const std::string& option_name)
      {
      return this->service.process_has_option(this->implementation, option_name);
      }

    boost::program_options::variable_value get_process_option(
        const std::string& option_name)
      {
      return this->service.get_process_option(this->implementation, option_name);
      }

    int process_start(thread_callback handler)
      {
      return this->service.process_start(this->implementation, handler);
      }

    void set_watchdog(uint32_t seconds)
      {
      return this->service.set_watchdog(this->implementation, seconds);
      }
  };

}; // namespace apoa

#endif // LIBAPOA_BASIC_APPLICATION_HANDLER_HPP
