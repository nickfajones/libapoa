/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <boost/shared_ptr.hpp>

#include <libapoa/basic_thread_handler_service.hpp>
#include <libapoa/thread_handler_impl.hpp>

namespace apoa
{

//#############################################################################
boost::shared_ptr<thread_handler_impl> _thread_handler_impl;



//#############################################################################
basic_thread_handler_service::basic_thread_handler_service(
    boost::asio::io_service& io_service) :
  boost::asio::detail::service_base<
    basic_thread_handler_service>(io_service)
  {
  }

basic_thread_handler_service::~basic_thread_handler_service()
  {
  }

//#############################################################################
void basic_thread_handler_service::shutdown_service()
  {
  }

void basic_thread_handler_service::construct(implementation_type& impl)
  {
  if (_thread_handler_impl.use_count() == 0)
    {
    _thread_handler_impl.reset(
      new thread_handler_impl());
    }
  
  impl = _thread_handler_impl;
  }

void basic_thread_handler_service::destroy(implementation_type& impl)
  {
  impl.reset();
  
  if (_thread_handler_impl.use_count() == 1)
    {
    _thread_handler_impl.reset();
    }
  }

//#############################################################################
boost::asio::io_service& basic_thread_handler_service::get_thread_io_service(
    implementation_type& impl, pid_t tid)
  {
  return impl->get_io_service(tid);
  }

//#############################################################################
void basic_thread_handler_service::shutdown_thread(
    implementation_type& impl, pid_t tid, int retval)
  {
  impl->shutdown_thread(tid, retval);
  }

//#############################################################################
int basic_thread_handler_service::get_process_retval(
    implementation_type& impl)
  {
  return impl->get_process_retval();
  }

}; // namespace apoa
