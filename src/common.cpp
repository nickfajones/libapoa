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

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#ifdef __APPLE__
#include <mach/mach_init.h>
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
#if defined __linux__
__thread pid_t _apoa_linux_tid = 0;
#endif
#if defined __APPLE__
pid_t _apoa_apple_pid = pthread_mach_thread_np(pthread_self());
#endif

pid_t get_pid()
  {
#if defined __linux__
  return getpid();
#elif defined __APPLE__
  return _apoa_apple_pid;
#else
#error get_pid: unsupported platform
#endif
  }

pid_t get_tid()
  {
#if defined __linux__
  if (_apoa_linux_tid == 0)
    {
    _apoa_linux_tid = (pid_t)syscall(__NR_gettid);
    }
  return _apoa_linux_tid;
#elif defined __APPLE__
  return pthread_mach_thread_np(pthread_self());
#else
#error "get_tid: unsupported platform"
#endif
  }

bool is_process_thread()
  {
  return (get_tid() == get_pid());
  }

}; // namespace apoa
