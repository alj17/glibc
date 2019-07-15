/* clock_gettime -- Get current time from a POSIX clockid_t.  Linux version.
   Copyright (C) 2003-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <errno.h>
#include <time.h>
#include "kernel-posix-cpu-timers.h"

#ifdef HAVE_CLOCK_GETTIME_VSYSCALL
# define HAVE_VSYSCALL
#endif
#include <sysdep-vdso.h>

/* Get current value of CLOCK and store it in TP.  */

#if __WORDSIZE == 32
int
__clock_gettime (clockid_t clock_id, struct timespec *tp)
{
   int ret;

#ifdef __NR_clock_gettime64
  struct __timespec64 tp64;
  ret = INLINE_VSYSCALL (clock_gettime64, 2, clock_id, &tp64);

  tp->tv_sec = tp64.tv_sec;
  tp->tv_nsec = tp64.tv_nsec;

  if (! in_time_t_range (tp->tv_sec))
    {
      __set_errno (EOVERFLOW);
      return -1;
    }
#endif

#ifndef __ASSUME_TIME64_SYSCALLS
  ret = INLINE_VSYSCALL (clock_gettime, 2, clock_id, tp);
#endif

  return ret;
}
#else
int
__clock_gettime (clockid_t clock_id, struct timespec *tp)
{
  return INLINE_VSYSCALL (clock_gettime, 2, clock_id, tp);
}
#endif

weak_alias (__clock_gettime, clock_gettime)
libc_hidden_def (__clock_gettime)
