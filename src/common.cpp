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

#include "common.hpp"
#include "thread_handler.hpp"


namespace apoa
{

namespace priv
{

//#############################################################################
int32_t per_thread_registry::index_id_(0);

boost::mutex per_thread_registry::callback_mutex_;

per_thread_registry::per_thread_index_callback_map
  per_thread_registry::thread_start_callbacks_;
per_thread_registry::per_thread_index_callback_map
  per_thread_registry::thread_finish_callbacks_;

std::set<tenum_t> per_thread_registry::thread_set_;

};



//#############################################################################
per_thread_index<thread_handler::thread_registration>
  thread_handler::thread_registry_(-1);

boost::mutex thread_handler::tid_tenum_map_mutex_;
std::map<pid_t, tenum_t> thread_handler::tid_tenum_map_;



//#############################################################################
boost::asio::io_service& get_process_io_service()
  {
  return thread_handler::get_io_service(0);
  }

void shutdown_process(int retval)
  {
  thread_handler::shutdown_thread(0, retval);
  }

//#############################################################################
boost::asio::io_service& get_io_service(apoa::tenum_t tenum)
  {
  return thread_handler::get_io_service(tenum);
  }

void shutdown_thread(apoa::tenum_t tenum, int retval)
  {
  thread_handler::shutdown_thread(tenum, retval);
  }

//#############################################################################
boost::asio::io_service& get_io_service(pid_t tid)
  {
  return thread_handler::get_io_service_tid(tid);
  }

void shutdown_thread(pid_t tid, int retval)
  {
  thread_handler::shutdown_thread_tid(tid, retval);
  }

}; // namespace apoa
