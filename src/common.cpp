/*#############################################################################
#
# Copyright (C) 2011 Network Box Corporation Limited
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################
 */

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef __APPLE__
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#endif

#include "common.hpp"


namespace apoa
{

//#############################################################################
int _log_level = 1000000;

#define _APOA_LOG_FMT_SIZE                               (25 + 256)
#if defined __linux__
__thread char _log_fmt[_APOA_LOG_FMT_SIZE + 1];
#endif

void set_log_level(log_level level)
  {
  _log_level = level;
  }

void log(log_level level, const char* fmt, ...)
  {
  va_list ap;
  
  if ((_log_level == 0) || (level > _log_level))
    {
    return;
    }
  
  struct timeval tv;
  gettimeofday(&tv, NULL);
  
  char tm_buf[10];
  if (strftime(tm_buf, 9, "%T", localtime(&tv.tv_sec)) == 0) \
    {
    printf("[%05d] unable to generate apoa::log timestamp\n", get_tid());
    fflush(stdout);
    
    return;
    }
  
  if (strlen(fmt) >= _APOA_LOG_FMT_SIZE)
    {
    printf("[%05d] %s:%06ld apoa::log format string too long\n",
      get_tid(), tm_buf, tv.tv_usec);
    fflush(stdout);
    
    return;
    }
  
#if defined __APPLE__
  char _log_fmt[_APOA_LOG_FMT_SIZE];
#endif
  sprintf(_log_fmt, "[%05d] %s:%06ld %s\n",
    get_tid(), tm_buf, tv.tv_usec, fmt);
  
  va_start(ap, fmt);
  vprintf(_log_fmt, ap);
  va_end(ap);
  fflush(stdout);
  }

#undef _APOA_LOG_FMT_SIZE


//#############################################################################
#ifdef __linux__
__thread pid_t _apoa_tid = 0;
#endif

pid_t get_tid()
  {
#if defined __linux__
  if (_apoa_tid == 0)
    {
    _apoa_tid = (pid_t)syscall(__NR_gettid);
    }
  
  return _apoa_tid;
#elif defined __APPLE__
  int apoa_tid = mach_thread_self();
  mach_port_deallocate(mach_task_self(), apoa_tid);
  return apoa_tid;
#else
#error "No support for get_tid on this platform"
#endif
  }

}; // namespace apoa
