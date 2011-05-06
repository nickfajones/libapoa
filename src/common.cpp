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
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "common.hpp"


namespace apoa
{

//#############################################################################
int _log_level = 1000000;

#define _APOA_LOG_FMT_SIZE                               (25 + 256)
__thread char _log_fmt[_APOA_LOG_FMT_SIZE + 1];

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
  
  sprintf(_log_fmt, "[%05d] %s:%06ld %s\n",
    get_tid(), tm_buf, tv.tv_usec, fmt);
  
  va_start(ap, fmt);
  vprintf(_log_fmt, ap);
  va_end(ap);
  fflush(stdout);
  }



//#############################################################################
__thread pid_t _apoa_tid = 0;

pid_t get_tid()
  {
  if (_apoa_tid == 0)
    {
    _apoa_tid = (pid_t)syscall(__NR_gettid);
    }
  
  return _apoa_tid;
  }

}; // namespace apoa
