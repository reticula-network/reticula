#include <iostream>

#include <catch2/catch.hpp>

#include <dag/intervals.hpp>

TEST_CASE("interval set", "[dag::interval_set]") {
  /* SECTION("can be initialized with a range of intervals") { */
  /*   dag::interval_set<int> is1{{0, 3}, {5, 8}}; */
  /*   REQUIRE(is1.cover() == 6) */

  /*   dag::interval_set<int> is2{{0, 6}, {5, 8}}; */
  /*   REQUIRE(is2.cover() == 8) */

  /*   dag::interval_set<int> is3{{5, 8}, {0, 6}}; */
  /*   REQUIRE(is3.cover() == 8) */
  /* } */

  SECTION("insert") {
    dag::interval_set<int> is;

    is.insert(0, 2);
    is.insert(1, 4);
    is.insert(5, 7);
    is.insert(6, 7);
    is.insert(7, 7);
    is.insert(-4, -1);
    is.insert(-3, -2);

    REQUIRE(is.cover() == 9);

    REQUIRE_FALSE(is.is_covered(-4));
    REQUIRE(      is.is_covered(-2));
    REQUIRE(      is.is_covered(-3));
    REQUIRE_FALSE(is.is_covered(0));
    REQUIRE(      is.is_covered(2));
    REQUIRE(      is.is_covered(4));
    REQUIRE_FALSE(is.is_covered(5));
    REQUIRE(      is.is_covered(7));
    REQUIRE_FALSE(is.is_covered(8));
  }

  SECTION("merge") {
    dag::interval_set<int> is1;

    is1.insert(0, 2);
    is1.insert(1, 4);
    is1.insert(5, 7);
    is1.insert(6, 7);
    is1.insert(7, 7);
    is1.insert(-4, -1);
    is1.insert(-3, -2);

    SECTION("empty") {
      dag::interval_set<int> is2;
      is2.merge(is1);
      REQUIRE(is1 == is2);

      dag::interval_set<int> is3;
      is1.merge(is3);
      REQUIRE(is1 == is2);
    }

    SECTION("identical") {
      dag::interval_set<int> is2(is1);
      is1.merge(is2);
      REQUIRE(is1 == is2);
    }

    SECTION("overlapping") {
      dag::interval_set<int> is2;

      is2.insert(-5, -2);
      is2.insert(6, 12);

      dag::interval_set<int> original_is2(is2);
      is2.merge(is1);
      REQUIRE(std::ranges::all_of(std::views::iota(-10, 20),
            [&is1, &is2, &original_is2](int i) {
              return (is1.is_covered(i) | original_is2.is_covered(i)) ==
                (is2.is_covered(i));
              }));
    }
  }
}
