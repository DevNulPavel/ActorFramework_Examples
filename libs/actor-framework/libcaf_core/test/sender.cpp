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

#define CAF_SUITE sender

#include "caf/mixin/sender.hpp"

#include "caf/test/dsl.hpp"

#include <chrono>

using namespace caf;

using std::chrono::seconds;

namespace {

behavior testee_impl(event_based_actor* self) {
  self->set_default_handler(drop);
  return {[] {
    // nop
  }};
}

struct fixture : test_coordinator_fixture<> {
  group grp;
  actor testee;

  fixture() {
    grp = sys.groups().anonymous();
    testee = sys.spawn_in_group(grp, testee_impl);
  }

  ~fixture() {
    anon_send_exit(testee, exit_reason::user_shutdown);
  }
};

} // namespace

CAF_TEST_FIXTURE_SCOPE(sender_tests, fixture)

CAF_TEST(delayed actor message) {
  self->delayed_send(testee, seconds(1), "hello world");
  sched.trigger_timeout();
  expect((std::string), from(self).to(testee).with("hello world"));
}

CAF_TEST(delayed group message) {
  self->delayed_send(grp, seconds(1), "hello world");
  sched.trigger_timeout();
  expect((std::string), from(self).to(testee).with("hello world"));
}

CAF_TEST(scheduled actor message) {
  self->scheduled_send(testee, self->clock().now() + seconds(1), "hello world");
  sched.trigger_timeout();
  expect((std::string), from(self).to(testee).with("hello world"));
}

CAF_TEST(scheduled group message) {
  self->scheduled_send(grp, self->clock().now() + seconds(1), "hello world");
  sched.trigger_timeout();
  expect((std::string), from(self).to(testee).with("hello world"));
}

CAF_TEST_FIXTURE_SCOPE_END()
