// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Modified by Ivan Prisyazhyy <john.koepi@gmail.com>

#include "time.h"
#include "macros.h"

// unistd required for _POSIX_MONOTONIC_CLOCK
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

namespace v8 {
namespace base {

int64_t FromTimeval(struct timeval tv) {
  DCHECK_GE(tv.tv_usec, 0);
  DCHECK(tv.tv_usec < static_cast<suseconds_t>(TimeConstants::kMicrosecondsPerSecond));
  if (tv.tv_usec == 0 && tv.tv_sec == 0) {
    return 0;
  }

  if (tv.tv_usec == static_cast<suseconds_t>(TimeConstants::kMicrosecondsPerSecond - 1) &&
      tv.tv_sec == std::numeric_limits<time_t>::max()) {
    return std::numeric_limits<int64_t>::max();
  }

  return tv.tv_sec * TimeConstants::kMicrosecondsPerSecond + tv.tv_usec;
}


// equivalent to Clock::GetSystemTime()
int64_t NowFromSystemTime() {
  struct timeval tv;
  int result = gettimeofday(&tv, nullptr);
  DCHECK_EQ(0, result);
  USE(result);
  return FromTimeval(tv);
}


// Helper function to get results from clock_gettime() and convert to a
// microsecond timebase. Minimum requirement is MONOTONIC_CLOCK to be supported
// on the system. FreeBSD 6 has CLOCK_MONOTONIC but defines
// _POSIX_MONOTONIC_CLOCK to -1.
int64_t ClockNow(clockid_t clk_id) {
#if (defined(_POSIX_MONOTONIC_CLOCK) && _POSIX_MONOTONIC_CLOCK >= 0) || \
  defined(V8_OS_BSD) || defined(V8_OS_ANDROID)
#if defined(V8_OS_AIX)
  // On AIX clock_gettime for CLOCK_THREAD_CPUTIME_ID outputs time with
  // resolution of 10ms. thread_cputime API provides the time in ns.
  if (clk_id == CLOCK_THREAD_CPUTIME_ID) {
#if defined(__PASE__)  // CLOCK_THREAD_CPUTIME_ID clock not supported on IBMi
    return 0;
#else
    thread_cputime_t tc;
    if (thread_cputime(-1, &tc) != 0) {
      UNREACHABLE();
    }
    return (tc.stime / TimeConstants::kNanosecondsPerMicrosecond)
           + (tc.utime / TimeConstants::kNanosecondsPerMicrosecond);
#endif  // defined(__PASE__)
  }
#endif  // defined(V8_OS_AIX)
  struct timespec ts;
  if (clock_gettime(clk_id, &ts) != 0) {
    UNREACHABLE();
  }
  // Multiplying the seconds by {kMicrosecondsPerSecond}, and adding something
  // in [0, kMicrosecondsPerSecond) must result in a valid {int64_t}.
  static constexpr int64_t kSecondsLimit =
      (std::numeric_limits<int64_t>::max() /
       TimeConstants::kMicrosecondsPerSecond) -
      1;
  CHECK_GT(kSecondsLimit, ts.tv_sec);
  int64_t result = int64_t{ts.tv_sec} * TimeConstants::kMicrosecondsPerSecond;
  result += (ts.tv_nsec / TimeConstants::kNanosecondsPerMicrosecond);
  return result;
#else  // Monotonic clock not supported.
  return 0;
#endif
}


int64_t TimeTicksNow() {
  // Make sure we never return 0 here.
  return ClockNow(CLOCK_MONOTONIC) + 1;
}


int64_t NanosecondsNow() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return int64_t{ts.tv_sec} * TimeConstants::kNanosecondsPerSecond +
         ts.tv_nsec;
}

bool IsHighResolutionTimer(clockid_t clk_id) {
  // Currently this is only needed for CLOCK_MONOTONIC. If other clocks need
  // to be checked, care must be taken to support all platforms correctly;
  // see ClockNow() above for precedent.
  DCHECK_EQ(clk_id, CLOCK_MONOTONIC);
  int64_t previous = NanosecondsNow();
  // There should be enough attempts to make the loop run for more than one
  // microsecond if the early return is not taken -- the elapsed time can't
  // be measured in that situation, so we have to estimate it offline.
  constexpr int kAttempts = 100;
  for (int i = 0; i < kAttempts; i++) {
    int64_t next = NanosecondsNow();
    int64_t delta = next - previous;
    if (delta == 0) continue;
    // We expect most systems to take this branch on the first iteration.
    if (delta <= TimeConstants::kNanosecondsPerMicrosecond) {
      return true;
    }
    previous = next;
  }
  // As of 2022, we expect that the loop above has taken at least 2 μs (on
  // a fast desktop). If we still haven't seen a non-zero clock increment
  // in sub-microsecond range, assume a low resolution timer.
  return false;
}


}  // namespace base
}  // namespace v8
