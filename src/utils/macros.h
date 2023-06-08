// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MACROS_H_
#define MACROS_H_

#include <limits>
#include <type_traits>

// No-op macro which is used to work around MSVC's funky VA_ARGS support.
#define EXPAND(x) x

// This macro does nothing. That's all.
#define NOTHING(...)

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
// Creates an unique identifier. Useful for scopes to avoid shadowing names.
#define UNIQUE_IDENTIFIER(base) CONCAT(base, __COUNTER__)

namespace v8 {
namespace base {

// The USE(x, ...) template is used to silence C++ compiler warnings
// issued for (yet) unused variables (typically parameters).
// The arguments are guaranteed to be evaluated from left to right.
struct Use {
  template <typename T>
  Use(T&&) {}  // NOLINT(runtime/explicit)
};
#define USE(...)                                                   \
  do {                                                             \
    ::v8::base::Use unused_tmp_array_for_use_macro[]{__VA_ARGS__}; \
    (void)unused_tmp_array_for_use_macro;                          \
  } while (false)

}  // namespace base
}  // namespace v8


// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

namespace node {

// The reason that Assert() takes a struct argument instead of individual
// const char*s is to ease instruction cache pressure in calls from CHECK.
struct AssertionInfo {
  const char* file_line;  // filename:line
  const char* message;
  const char* function;
};
[[noreturn]] extern void Abort(); // extern, from node.js
[[noreturn]] extern void Assert(const AssertionInfo& info); // extern, from node.js
#define ABORT() node::Abort()

#define ERROR_AND_ABORT(expr)                                                 \
  do {                                                                        \
    /* Make sure that this struct does not end up in inline code, but      */ \
    /* rather in a read-only data section when modifying this code.        */ \
    static const node::AssertionInfo args = {                             \
      __FILE__ ":" STRINGIFY(__LINE__), #expr, PRETTY_FUNCTION_NAME           \
    };                                                                        \
    node::Assert(args);                                                       \
  } while (0)

#ifdef __GNUC__
#define LIKELY(expr) __builtin_expect(!!(expr), 1)
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#define PRETTY_FUNCTION_NAME __PRETTY_FUNCTION__
#else
#define LIKELY(expr) expr
#define UNLIKELY(expr) expr
#define PRETTY_FUNCTION_NAME ""
#endif

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#define CHECK(expr)                                                           \
  do {                                                                        \
    if (UNLIKELY(!(expr))) {                                                  \
      ERROR_AND_ABORT(expr);                                                  \
    }                                                                         \
  } while (0)

#define CHECK_EQ(a, b) CHECK((a) == (b))
#define CHECK_GE(a, b) CHECK((a) >= (b))
#define CHECK_GT(a, b) CHECK((a) > (b))
#define CHECK_LE(a, b) CHECK((a) <= (b))
#define CHECK_LT(a, b) CHECK((a) < (b))
#define CHECK_NE(a, b) CHECK((a) != (b))
#define CHECK_NULL(val) CHECK((val) == nullptr)
#define CHECK_NOT_NULL(val) CHECK((val) != nullptr)
#define CHECK_IMPLIES(a, b) CHECK(!(a) || (b))

#ifdef DEBUG
  #define DCHECK(expr) CHECK(expr)
  #define DCHECK_EQ(a, b) CHECK((a) == (b))
  #define DCHECK_GE(a, b) CHECK((a) >= (b))
  #define DCHECK_GT(a, b) CHECK((a) > (b))
  #define DCHECK_LE(a, b) CHECK((a) <= (b))
  #define DCHECK_LT(a, b) CHECK((a) < (b))
  #define DCHECK_NE(a, b) CHECK((a) != (b))
  #define DCHECK_NULL(val) CHECK((val) == nullptr)
  #define DCHECK_NOT_NULL(val) CHECK((val) != nullptr)
  #define DCHECK_IMPLIES(a, b) CHECK(!(a) || (b))
#else
  #define DCHECK(expr)
  #define DCHECK_EQ(a, b)
  #define DCHECK_GE(a, b)
  #define DCHECK_GT(a, b)
  #define DCHECK_LE(a, b)
  #define DCHECK_LT(a, b)
  #define DCHECK_NE(a, b)
  #define DCHECK_NULL(val)
  #define DCHECK_NOT_NULL(val)
  #define DCHECK_IMPLIES(a, b)
#endif

#define UNREACHABLE(...)                                                      \
  ERROR_AND_ABORT("Unreachable code reached" __VA_OPT__(": ") __VA_ARGS__)

}  // namespace node

#endif  // MACROS_H_
