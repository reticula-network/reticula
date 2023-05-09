#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <reticula/ranges.hpp>
#include <reticula/intervals.hpp>

TEST_CASE("interval set", "[reticula::interval_set]") {
  SECTION("insert 1") {
    reticula::interval_set<int> is;
    is.insert(1, 6);
    is.insert(8, 10);
    is.insert(3, 9);

    REQUIRE(reticula::ranges::is_sorted(is));
    REQUIRE(reticula::ranges::is_sorted(is, reticula::ranges::less{},
          [](auto p){ return p.second; }));
    REQUIRE(reticula::ranges::size(is) == 1);
  }

  SECTION("insert 2") {
    reticula::interval_set<int> is;

    is.insert(0, 2);
    is.insert(1, 4);
    is.insert(5, 7);
    is.insert(6, 7);
    is.insert(7, 7);
    is.insert(-4, -1);
    is.insert(-3, -2);

    REQUIRE(is.cover() == 9);
    REQUIRE(reticula::ranges::is_sorted(is));

    REQUIRE_FALSE(is.covers(-4));
    REQUIRE_FALSE(is.covers(0));
    REQUIRE_FALSE(is.covers(5));
    REQUIRE_FALSE(is.covers(8));

    REQUIRE(is.covers(-2));
    REQUIRE(is.covers(-3));
    REQUIRE(is.covers(2));
    REQUIRE(is.covers(4));
    REQUIRE(is.covers(7));
  }

  SECTION("merge") {
    reticula::interval_set<int> is1;

    is1.insert(0, 2);
    is1.insert(1, 4);
    is1.insert(5, 7);
    is1.insert(6, 7);
    is1.insert(7, 7);
    is1.insert(-4, -1);
    is1.insert(-3, -2);

    SECTION("empty") {
      reticula::interval_set<int> is2;
      is2.merge(is1);
      REQUIRE(is1 == is2);

      reticula::interval_set<int> is3;
      is1.merge(is3);
      REQUIRE(is1 == is2);
      REQUIRE(reticula::ranges::is_sorted(is1));
    }

    SECTION("identical") {
      reticula::interval_set<int> is2(is1);
      is1.merge(is2);
      REQUIRE(is1 == is2);
      REQUIRE(reticula::ranges::is_sorted(is1));
    }

    SECTION("overlapping") {
      reticula::interval_set<int> is2;

      is2.insert(-5, -2);
      is2.insert(6, 12);

      reticula::interval_set<int> original_is2(is2);
      is2.merge(is1);
      REQUIRE(reticula::ranges::is_sorted(is2));
      REQUIRE(reticula::ranges::all_of(reticula::views::iota(-10, 20),
            [&is1, &is2, &original_is2](int i) {
              return (is1.covers(i) || original_is2.covers(i)) ==
                (is2.covers(i));
              }));
    }
  }
}
