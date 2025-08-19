#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

using Catch::Approx;

#include <reticula/interval_sets.hpp>

TEST_CASE("interval set", "[reticula::interval_set]") {
  SECTION("insert 1") {
    reticula::interval_set is;
    is.insert(1, 6);
    is.insert(8, 10);
    is.insert(3, 9);

    REQUIRE(std::ranges::is_sorted(is));
    REQUIRE(std::ranges::is_sorted(is, std::ranges::less{}, [](auto p) {
      return p.second;
    }));
    REQUIRE(std::ranges::size(is) == 1);
  }

  SECTION("insert 2") {
    reticula::interval_set is;

    is.insert(0, 2);
    is.insert(1, 4);
    is.insert(5, 7);
    is.insert(6, 7);
    is.insert(7, 7);
    is.insert(-4, -1);
    is.insert(-3, -2);

    REQUIRE(is.cover() == 9);
    REQUIRE(std::ranges::is_sorted(is));

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
    reticula::interval_set is1;

    is1.insert(0, 2);
    is1.insert(1, 4);
    is1.insert(5, 7);
    is1.insert(6, 7);
    is1.insert(7, 7);
    is1.insert(-4, -1);
    is1.insert(-3, -2);

    SECTION("empty") {
      reticula::interval_set is2;
      is2.merge(is1);
      REQUIRE(is1 == is2);

      reticula::interval_set is3;
      is1.merge(is3);
      REQUIRE(is1 == is2);
      REQUIRE(std::ranges::is_sorted(is1));
    }

    SECTION("identical") {
      reticula::interval_set is2(is1);
      is1.merge(is2);
      REQUIRE(is1 == is2);
      REQUIRE(std::ranges::is_sorted(is1));
    }

    SECTION("overlapping") {
      reticula::interval_set is2;

      is2.insert(-5, -2);
      is2.insert(6, 12);

      reticula::interval_set original_is2(is2);
      is2.merge(is1);
      REQUIRE(std::ranges::is_sorted(is2));
      REQUIRE(
        std::ranges::all_of(
          std::views::iota(-10, 20), [&is1, &is2, &original_is2](int i) {
            return (is1.covers(i) || original_is2.covers(i)) == (is2.covers(i));
          }));
    }
  }
}

TEST_CASE(
  "interval set cluster sketch", "[reticula::interval_set_cluster_sketch]") {
  SECTION("empty") {
    reticula::interval_set_cluster_sketch sketch(0.001, 42);
    REQUIRE(sketch.cover_estimate() == 0.0);
  }

  SECTION("insert and cover") {
    reticula::interval_set_cluster_sketch sketch(0.001, 42);

    sketch.insert(1, 0.0, 0.4);
    sketch.insert(1, 0.2, 0.5);
    sketch.insert(2, 0.3, 0.7);
    sketch.insert(2, 0.3, 0.7);
    sketch.insert(2, 0.3, 0.3);
    sketch.insert(3, 0.6, 1.0);
    sketch.insert(1, 1.0, 2.0);

    // 1: 0.0..0.5 1.0..2.0
    // 2: 0.3..0.7
    // 3: 0.6..1.0
    // 1.5 + 0.4 + 0.4 = 2.3

    REQUIRE(sketch.cover_estimate() == Approx(2.3).margin(0.05));
  }

  SECTION("insert with NaN") {
    reticula::interval_set_cluster_sketch sketch(0.001, 42);

    REQUIRE_THROWS_AS(
      sketch.insert(1, 0.0, std::nan("")), std::invalid_argument);
    REQUIRE_THROWS_AS(
      sketch.insert(2, std::nan(""), 0.5), std::invalid_argument);
    REQUIRE_NOTHROW(sketch.insert(3, 0.1, 0.2));
  }

  SECTION("insert with infinite start or end") {
    reticula::interval_set_cluster_sketch sketch1(0.001, 42);
    sketch1.insert(1, 0.0, std::numeric_limits<double>::infinity());
    sketch1.insert(1, -1.0, 2.0);
    sketch1.insert(1, 2.0, 4.0);
    REQUIRE(sketch1.cover_estimate() == std::numeric_limits<double>::infinity());

    reticula::interval_set_cluster_sketch sketch2(0.001, 42);
    sketch2.insert(1, -std::numeric_limits<double>::infinity(), 12);
    sketch1.insert(1, -1.0, 2.0);
    sketch1.insert(1, 2.0, 4.0);
    REQUIRE(sketch2.cover_estimate() == std::numeric_limits<double>::infinity());
  }

  SECTION("merge") {
    REQUIRE(
      reticula::hash<reticula::VertexType>{}(reticula::VertexType{1}) !=
      reticula::hash<reticula::VertexType>{}(reticula::VertexType{2}));

    using BucketID = reticula::interval_set_cluster_sketch::BucketId;
    REQUIRE(
      reticula::hash<BucketID>{}(BucketID{1}) !=
      reticula::hash<BucketID>{}(BucketID{2}));

    const auto ph1 = hll::hash<std::pair<
      reticula::VertexType, reticula::interval_set_cluster_sketch::BucketId>>{}(
      {reticula::VertexType{1}, 42}, 123);
    const auto ph2 = hll::hash<std::pair<
      reticula::VertexType, reticula::interval_set_cluster_sketch::BucketId>>{}(
      {reticula::VertexType{2}, 42}, 123);
    REQUIRE(ph1 != ph2);

    reticula::interval_set_cluster_sketch sketch1(0.001, 42);
    sketch1.insert(1, 0.0, 0.5);
    sketch1.insert(2, 0.3, 0.7);
    sketch1.insert(4, 1.2, 1.5);

    reticula::interval_set_cluster_sketch sketch2(0.001, 42);
    sketch2.insert(1, 0.2, 1.0);
    sketch2.insert(3, 0.6, 1.0);
    sketch2.insert(4, 1.2, 1.5);

    // 1: 0.0..1.0
    // 2: 0.3..0.7
    // 3: 0.6..1.0
    // 4: 1.2..1.5
    // // 1.0 + 0.4 + 0.4 + 0.3 = 2.1

    sketch1.merge(sketch2);

    REQUIRE(sketch1.cover_estimate() == Approx(2.1).margin(0.05));
  }
}
