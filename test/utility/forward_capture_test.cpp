/** @file
 *
 *  @ingroup test_module
 *
 *  @brief Test scenarios for Vittorio Romeo's perfect forward capture utilities.
 *
 *  @author Cliff Green
 *
 *  Copyright (c) 2019 by Cliff Green
 *
 *  Distributed under the Boost Software License, Version 1.0. 
 *  (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch2/catch.hpp"

#include <memory> // std::unique_ptr

#include "utility/forward_capture.hpp"

constexpr int base = 3;

struct copyable_foo {
  int val{base};
  int operator()(int i) { val += i; return val; }
};

struct movable_foo {
  std::unique_ptr<int> val{std::make_unique<int>(base)};
  int operator()(int i) { *val += i; return *val; }
};


template <typename F>
auto test_func(F&& f_obj, int val1, int val2) {
  return [func = CHOPS_FWD_CAPTURE(f_obj), val1, val2] () mutable {
    return chops::access(func)(val1) + chops::access(func)(val2);
  };
}


TEST_CASE( "Vittorio Romeo's perfect forward parameters for lambda capture utility",
          "[forward_capture]" ) {

  // lvalue reference captured, will modify original object
  {
    copyable_foo a{};
    auto lam = test_func(a, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    REQUIRE (a.val == 6);
    i = lam();
    REQUIRE (i == 16);
    REQUIRE (a.val == 9);
    i = lam();
    REQUIRE (i == 22);
    REQUIRE (a.val == 12);
  }
  // rvalue, copy made and stored in lambda
  {
    auto lam = test_func(copyable_foo{}, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    i = lam();
    REQUIRE (i == 16);
    i = lam();
    REQUIRE (i == 22);
  }
  // rvalue move captured
  {
    movable_foo a{};
    auto lam = test_func(a, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    i = lam();
    REQUIRE (i == 16);
    i = lam();
    REQUIRE (i == 22);
  }
  // rvalue move captured
  {
    auto lam = test_func(movable_foo{}, 1, 2);
    int i = lam();
    REQUIRE (i == 10);
    i = lam();
    REQUIRE (i == 16);
    i = lam();
    REQUIRE (i == 22);
  }
}
