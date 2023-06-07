// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_BASE_PLATFORM_TIME_H_
#define V8_BASE_PLATFORM_TIME_H_

#include <stdint.h>

#include <ctime>
#include <iosfwd>
#include <limits>
#include <sys/types.h>

// Forward declarations.
extern "C" {
struct _FILETIME;
struct mach_timespec;
struct timespec;
struct timeval;
}

namespace v8 {
namespace base {

class TimeConstants {
 public:
  static constexpr int64_t kHoursPerDay = 24;
  static constexpr int64_t kMillisecondsPerSecond = 1000;
  static constexpr int64_t kMillisecondsPerDay =
      kMillisecondsPerSecond * 60 * 60 * kHoursPerDay;
  static constexpr int64_t kMicrosecondsPerMillisecond = 1000;
  static constexpr int64_t kMicrosecondsPerSecond =
      kMicrosecondsPerMillisecond * kMillisecondsPerSecond;
  static constexpr int64_t kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
  static constexpr int64_t kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
  static constexpr int64_t kMicrosecondsPerDay =
      kMicrosecondsPerHour * kHoursPerDay;
  static constexpr int64_t kMicrosecondsPerWeek = kMicrosecondsPerDay * 7;
  static constexpr int64_t kNanosecondsPerMicrosecond = 1000;
  static constexpr int64_t kNanosecondsPerSecond =
      kNanosecondsPerMicrosecond * kMicrosecondsPerSecond;
};

// -----------------------------------------------------------------------------

// Returns the current time. Same as Now() except that this function always
// uses system time so that there are no discrepancies between the returned
// time and system time even on virtual environments including our test bot.
// For timing sensitive unittests, this function should be used.
int64_t NowFromSystemTime();

// Platform-dependent tick count representing "right now." When
// IsHighResolution() returns false, the resolution of the clock could be as
// coarse as ~15.6ms. Otherwise, the resolution should be no worse than one
// microsecond.
// This method never returns a null TimeTicks.
int64_t TimeTicksNow();

int64_t NanosecondsNow();

bool IsHighResolutionTimer(clockid_t clk_id);

}  // namespace base
}  // namespace v8

#endif  // V8_BASE_PLATFORM_TIME_H_
