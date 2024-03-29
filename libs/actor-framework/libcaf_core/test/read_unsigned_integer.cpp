/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2019 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#define CAF_SUITE read_unsigned_integer

#include "caf/detail/parser/read_unsigned_integer.hpp"

#include "caf/test/dsl.hpp"

#include "caf/detail/parser/state.hpp"
#include "caf/string_view.hpp"

using namespace caf;

namespace {

template <class T>
struct unsigned_integer_consumer {
  using value_type = T;

  void value(T y) {
    x = y;
  }

  T x;
};

template <class T>
optional<T> read(string_view str) {
  unsigned_integer_consumer<T> consumer;
  detail::parser::state<string_view::iterator> ps{str.begin(), str.end()};
  detail::parser::read_unsigned_integer(ps, consumer);
  if (ps.code != pec::success)
    return none;
  return consumer.x;
}

template <class T>
bool overflow(string_view str) {
  unsigned_integer_consumer<T> consumer;
  detail::parser::state<string_view::iterator> ps{str.begin(), str.end()};
  detail::parser::read_unsigned_integer(ps, consumer);
  return ps.code == pec::integer_overflow;
}

template <class T>
T max_val() {
  return std::numeric_limits<T>::max();
}

} // namespace

#define ZERO_VALUE(type, literal)                                              \
  CAF_CHECK_EQUAL(read<type>(#literal), type(0));

#define MAX_VALUE(type, literal)                                               \
  CAF_CHECK_EQUAL(read<type>(#literal), max_val<type>());

#ifdef OVERFLOW
#  undef OVERFLOW
#endif // OVERFLOW

#define OVERFLOW(type, literal) CAF_CHECK(overflow<type>(#literal));

CAF_TEST(read zeros) {
  ZERO_VALUE(uint8_t, 0);
  ZERO_VALUE(uint8_t, 00);
  ZERO_VALUE(uint8_t, 0x0);
  ZERO_VALUE(uint8_t, 0X00);
  ZERO_VALUE(uint8_t, 0b0);
  ZERO_VALUE(uint8_t, 0B00);
  ZERO_VALUE(uint8_t, +0);
  ZERO_VALUE(uint8_t, +00);
  ZERO_VALUE(uint8_t, +0x0);
  ZERO_VALUE(uint8_t, +0X00);
  ZERO_VALUE(uint8_t, +0b0);
  ZERO_VALUE(uint8_t, +0B00);
}

CAF_TEST(maximal value) {
  MAX_VALUE(uint8_t, 0b11111111);
  MAX_VALUE(uint8_t, 0377);
  MAX_VALUE(uint8_t, 255);
  MAX_VALUE(uint8_t, 0xFF);
  OVERFLOW(uint8_t, 0b100000000);
  OVERFLOW(uint8_t, 0400);
  OVERFLOW(uint8_t, 256);
  OVERFLOW(uint8_t, 0x100);
  MAX_VALUE(uint16_t, 0b1111111111111111);
  MAX_VALUE(uint16_t, 0177777);
  MAX_VALUE(uint16_t, 65535);
  MAX_VALUE(uint16_t, 0xFFFF);
  OVERFLOW(uint16_t, 0b10000000000000000);
  OVERFLOW(uint16_t, 0200000);
  OVERFLOW(uint16_t, 65536);
  OVERFLOW(uint16_t, 0x10000);
  MAX_VALUE(uint32_t, 0b11111111111111111111111111111111);
  MAX_VALUE(uint32_t, 037777777777);
  MAX_VALUE(uint32_t, 4294967295);
  MAX_VALUE(uint32_t, 0xFFFFFFFF);
  OVERFLOW(uint32_t, 0b100000000000000000000000000000000);
  OVERFLOW(uint32_t, 040000000000);
  OVERFLOW(uint32_t, 4294967296);
  OVERFLOW(uint32_t, 0x100000000);
  MAX_VALUE(uint64_t,
            0b1111111111111111111111111111111111111111111111111111111111111111);
  MAX_VALUE(uint64_t, 01777777777777777777777);
  MAX_VALUE(uint64_t, 18446744073709551615);
  MAX_VALUE(uint64_t, 0xFFFFFFFFFFFFFFFF);
  OVERFLOW(uint64_t,
           0b10000000000000000000000000000000000000000000000000000000000000000);
  OVERFLOW(uint64_t, 02000000000000000000000);
  OVERFLOW(uint64_t, 18446744073709551616);
  OVERFLOW(uint64_t, 0x10000000000000000);
}
