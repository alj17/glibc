/* Linux nanosleep syscall implementation.
   Copyright (C) 2017-2019 Free Software Foundation, Inc.
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

#include <time.h>
#include <sysdep-cancel.h>
#include <not-cancel.h>

#if defined(__ASSUME_TIME64_SYSCALLS) || defined(__NR_clock_nanosleep_time64)
static int
__nanosleep_time64_64 (const struct timespec *requested_time,
                       struct timespec *remaining)
{
  return SYSCALL_CANCEL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                         requested_time, remaining);
}

#if __TIMESIZE == 32
struct timespec64
{
  long int tv_sec;   /* Seconds.  */
  long int tv_nsec;  /* Nanoseconds.  */
};

static int
__nanosleep_time64_32 (const struct timespec *requested_time,
                       struct timespec *remaining)
{
  timespec64 ts;

  long int ret = SYSCALL_CANCEL (clock_nanosleep_time64, CLOCK_REALTIME, 0,
                                 requested_time, &ts);

  remaining->tv_sec = ts.tv_sec;
  remaining->tv_nsec = ts.tv_nsec;

  return ret;
}
#endif
#endif

/* Pause execution for a number of nanoseconds.  */
int
__nanosleep (const struct timespec *requested_time,
	     struct timespec *remaining)
{
#ifdef __ASSUME_TIME64_SYSCALLS
  return __nanosleep_time64_64 (requested_time, remaining);
#else
# ifdef __NR_clock_nanosleep_time64
#  if __TIMESIZE == 64
  long int ret = __nanosleep_time64_64 (requested_time, remaining);

  if (ret == 0 || errno != ENOSYS)
    return ret;
#  else
  long int ret = __nanosleep_time64_32 (requested_time, remaining);

  if (ret == 0 || errno != ENOSYS)
    return ret;
#  endif
# endif
  return SYSCALL_CANCEL (nanosleep, requested_time, remaining);
#endif
}
hidden_def (__nanosleep)
weak_alias (__nanosleep, nanosleep)
