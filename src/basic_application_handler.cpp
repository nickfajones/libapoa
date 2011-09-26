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

#include <boost/assert.hpp>

#include <libapoa/basic_application_handler.hpp>



namespace apoa
{

//#############################################################################
basic_application_handler::basic_application_handler(
    boost::asio::io_service& io_service) :
  boost::asio::basic_io_object<basic_application_handler_service>(io_service),
  is_started_(false)
  {
  }

basic_application_handler::~basic_application_handler()
  {
  }

//#############################################################################
void basic_application_handler::process_init(
    boost::program_options::options_description& options_desc,
    int argc, char** argv)
  {
  service.process_init(implementation, options_desc, argc, argv);
  }

//#############################################################################
boost::asio::io_service& basic_application_handler::get_process_io_service()
  {
  return service.get_process_io_service(implementation);
  }

//#############################################################################
void basic_application_handler::set_watchdog(uint32_t seconds)
  {
  return service.set_watchdog(implementation, seconds);
  }

//#############################################################################
void basic_application_handler::shutdown_process(int retval)
  {
  service.shutdown_process(implementation, retval);
  }

//#############################################################################
void basic_application_handler::pre_start()
  {
  if (is_started_)
    {
    BOOST_ASSERT(0 && "basic_application_handler: "
      "process already started");
    }
  
  is_started_ = true;
  }

}; // namespace apoa
