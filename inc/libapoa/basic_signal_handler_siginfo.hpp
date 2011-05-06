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

#ifndef LIBAPOA_BASIC_SIGNAL_HANDLER_SIGINFO_HPP
#define LIBAPOA_BASIC_SIGNAL_HANDLER_SIGINFO_HPP

#include <stdint.h>


namespace apoa
{

//#############################################################################
#ifdef __linux__
struct basic_siginfo
  {
  uint32_t  bsi_signo;
  int32_t   bsi_errno;
  int32_t   bsi_code;
  uint32_t  bsi_pid;
  uint32_t  bsi_uid;
  int32_t   bsi_fd;
  uint32_t  bsi_tid;
  uint32_t  bsi_band;
  uint32_t  bsi_overrun;
  uint32_t  bsi_trapno;
  int32_t   bsi_status;
  int32_t   bsi_int;
  uint64_t  bsi_ptr;
  uint64_t  bsi_utime;
  uint64_t  bsi_stime;
  uint64_t  bsi_addr;
  
  uint32_t  bsi_ttid;
  
  uint8_t   bsi_pad[44];
  };

#else ifdef __APPLE__

union basic_sigval
  {
  int bsi_sival_int;
  void* bsi_sival_ptr;
  };

struct basic_siginfo
  {
  int   bsi_signo;
  int   bsi_errno;
  int   bsi_code;
  pid_t bsi_pid;
  uid_t bsi_uid;
  int   bsi_status;
  void* bsi_addr;
  union basic_sigval bsi_value;
  long  bsi_band;
  
  unsigned long bsi_pad[7];
  };

#endif

}; // namespace apoa

#endif // LIBAPOA_BASIC_SIGNAL_HANDLER_SIGINFO_HPP
