/* C11 threads thread sleep implementation.
   Copyright (C) 2018-2019 Free Software Foundation, Inc.
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

#include "thrd_priv.h"

#if __TIMESIZE == 32
struct timespec64
{
  long int tv_sec;   /* Seconds.  */
  long int tv_nsec;  /* Nanoseconds.  */
};
#endif

int
thrd_sleep (const struct timespec* time_point, struct timespec* remaining)
{
  INTERNAL_SYSCALL_DECL (err);
  int ret;

#ifdef __ASSUME_TIME64_SYSCALLS
  ret = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err, CLOCK_REALTIME,
                                 0, time_point, remaining);
#else
# ifdef __NR_clock_nanosleep_time64
#  if __TIMESIZE == 64
  long int ret_64;

  ret_64 = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err, CLOCK_REALTIME,
                                    0, time_point, remaining);

  if (ret_64 == 0 || errno != ENOSYS)
    ret = ret_64;
#  else
  timespec64 ts;

  ret = INTERNAL_SYSCALL_CANCEL (clock_nanosleep_time64, err,
                                 CLOCK_REALTIME, 0, time_point,
                                 ts);

  if (ret == 0 || errno != ENOSYS) {
    remaining->tv_sec = ts.tv_sec;
    remaining->tv_nsec = ts.tv_nsec;
    return ret;
  }
#  endif
# endif
  ret =  INTERNAL_SYSCALL_CANCEL (nanosleep, err, time_point, remaining);
#endif

  if (INTERNAL_SYSCALL_ERROR_P (ret, err))
    {
      /* C11 states thrd_sleep function returns -1 if it has been interrupted
	 by a signal, or a negative value if it fails.  */
      ret = INTERNAL_SYSCALL_ERRNO (ret, err);
      if (ret == EINTR)
	return -1;
      return -2;
    }
  return 0;
}
