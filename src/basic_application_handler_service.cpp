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

#include <libapoa/basic_application_handler_service.hpp>

namespace apoa
{

//#############################################################################
boost::shared_ptr<application_handler_impl> _application_handler_impl;



//#############################################################################
basic_application_handler_service::basic_application_handler_service(
    boost::asio::io_service& io_service) :
  boost::asio::detail::service_base<
    basic_application_handler_service>(io_service),
  io_service_(io_service)
  {
  }

basic_application_handler_service::~basic_application_handler_service()
  {
  }

//#############################################################################
void basic_application_handler_service::shutdown_service()
  {
  }

void basic_application_handler_service::construct(implementation_type& impl)
  {
  if (_application_handler_impl.use_count() == 0)
    {
    _application_handler_impl.reset(
      new application_handler_impl(io_service_));
    }
  
  impl = _application_handler_impl;
  }

void basic_application_handler_service::destroy(implementation_type& impl)
  {
  impl.reset();
  }

//#############################################################################
void basic_application_handler_service::process_init(
  implementation_type& impl,
  boost::program_options::options_description& options_desc,
  int argc, char** argv)
  {
  impl->process_init(options_desc, argc, argv);
  }

//#############################################################################
int basic_application_handler_service::get_process_retval(
    implementation_type& impl)
  {
  return impl->get_process_retval();
  }

//#############################################################################
boost::asio::io_service& basic_application_handler_service::
  get_process_io_service(implementation_type& impl)
  {
  return impl->get_process_io_service();
  }

//#############################################################################
void basic_application_handler_service::set_watchdog(
    implementation_type& impl, uint32_t seconds)
  {
  impl->set_watchdog(seconds);
  }

//#############################################################################
void basic_application_handler_service::shutdown_process(
    implementation_type& impl, int retval)
  {
  impl->shutdown_process(retval);
  }
}; // namespace apoa
