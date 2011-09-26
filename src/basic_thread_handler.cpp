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

#include <sys/types.h>

#include <boost/assert.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include <libapoa/common.hpp>

#include <libapoa/basic_thread_handler.hpp>



namespace apoa
{

//#############################################################################
basic_thread_handler::basic_thread_handler(boost::asio::io_service& io_service) :
  boost::asio::basic_io_object<basic_thread_handler_service>(io_service)
  {
  }

basic_thread_handler::~basic_thread_handler()
  {
  }

//#############################################################################
boost::asio::io_service& basic_thread_handler::get_io_service(pid_t tid)
  {
  if (tid == 0)
    {
    tid = get_tid();
    }
  
  return service.get_thread_io_service(implementation, tid);
  }

//#############################################################################
void basic_thread_handler::shutdown_thread(pid_t tid, int retval)
  {
  service.shutdown_thread(implementation, tid, retval);
  }

//#############################################################################
void basic_thread_handler::register_main_thread(
    boost::asio::io_service& process_io_service,
    apoa::application_callback handler)
  {
  service.register_main_thread(implementation, process_io_service, handler);
  }

}; // namespace apoa
