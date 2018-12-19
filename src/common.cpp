/*#############################################################################
#
# Copyright (C) 2012 Network Box Corporation Limited
#   Nick Jones <nick.jones@network-box.com>
#   Jeff He <jeff.he@network-box.com>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include "common.hpp"
//#include "memory_pool_types.hpp"
#include "thread_handler.hpp"


namespace apoa
{

namespace priv
{
//#############################################################################
static __thread pid_t tid =                               -1;
static __thread int tenum =                               -1;

static int global_tenum =                                 -1;
  // force tenum of main thread to zero
};


//#############################################################################
pid_t get_pid()
  {
  return ::getpid();
  }

pid_t get_tid()
  {
  if (priv::tid == -1)
    {
    priv::tid = (pid_t)syscall(__NR_gettid);
    }

  return priv::tid;
  }

tenum_t get_tenum()
  {
  if (priv::tenum == -1)
    {
    priv::tenum = __sync_add_and_fetch(&priv::global_tenum, 1);
    }

  return tenum_t(priv::tenum);
  }

bool is_process_thread()
  {
  return (get_tid() == get_pid());
  }



namespace priv
{
//#############################################################################
static int log_level =                                    1000000;
};


//#############################################################################
int get_log_level()
  {
  return priv::log_level;
  }

void set_log_level(int level)
  {
  priv::log_level = level;
  }



namespace priv
{

//#############################################################################
int32_t per_thread_registry::index_id_(0);

std::mutex per_thread_registry::callback_mutex_;

per_thread_registry::per_thread_index_callback_map
  per_thread_registry::thread_start_callbacks_;
per_thread_registry::per_thread_index_callback_map
  per_thread_registry::thread_finish_callbacks_;

std::set<tenum_t> per_thread_registry::thread_set_;

};



//#############################################################################
asio::io_service& get_process_io_service()
  {
  return thread_handler::get_io_service(0);
  }

asio::io_service& get_thread_io_service()
  {
  return thread_handler::get_io_service_tid(get_tid());
  }

asio::io_service& get_io_service(apoa::tenum_t tenum)
  {
  return thread_handler::get_io_service(tenum);
  }

asio::io_service& get_io_service(pid_t tid)
  {
  return thread_handler::get_io_service_tid(tid);
  }

//#############################################################################
void shutdown_process(int retval)
  {
  thread_handler::shutdown_thread(0, retval);
  }

void shutdown_thread(apoa::tenum_t tenum, int retval)
  {
  thread_handler::shutdown_thread(tenum, retval);
  }

void shutdown_thread(pid_t tid, int retval)
  {
  thread_handler::shutdown_thread_tid(tid, retval);
  }

}; // namespace apoa
