#include <unordered_set>

#include <catch2/catch.hpp>
#include <hll/hyperloglog.hpp>

#include <dag/estimators.hpp>

TEST_CASE("HyperLogLog++ cardinality estimator",
    "[dag::hll_cardinality_estimator]") {
  STATIC_REQUIRE(dag::cardinality_estimator<
      dag::hll_cardinality_estimator<int, hll::hyperloglog<int, 18, 25>>>);
}

TEST_CASE("Unordered-set cardinality estimator",
    "[dag::unordered_set_cardinality_estimator]") {
  STATIC_REQUIRE(dag::cardinality_estimator<
      dag::unordered_set_cardinality_estimator<int>>);

  dag::unordered_set_cardinality_estimator<int>
    est(std::size_t{}, std::size_t{});
  est.insert(1);
  est.insert(2);

  SECTION("basic properties") {
    REQUIRE(est.size() == 2);
    REQUIRE(est.contains(1));
    REQUIRE_FALSE(est.contains(3));
    REQUIRE(est.underlying_estimator() ==
        std::unordered_set<int, dag::hash<int>>({1, 2}));
  }

  SECTION("insertion") {
    est.insert(3);
    REQUIRE(est.size() == 3);
    REQUIRE(est.contains(3));
    REQUIRE(est.underlying_estimator() ==
        std::unordered_set<int, dag::hash<int>>({1, 2, 3}));
  }

  SECTION("merging") {
    dag::unordered_set_cardinality_estimator<int>
      est2(std::size_t{}, std::size_t{});
    est2.insert(3);
    est2.insert(4);

    est.merge(est2);

    REQUIRE(est.size() == 4);
    REQUIRE(est.contains(4));
    REQUIRE(est.underlying_estimator() ==
        std::unordered_set<int, dag::hash<int>>({1, 2, 3, 4}));
  }
}

TEST_CASE("cardinality estimate", "[dag::cardinality_estimate]") {
  dag::hll_cardinality_estimator<int, hll::hyperloglog<int, 18, 25>> hest(
      std::size_t{}, std::size_t{});

  for (int i = 0; i < 100; i++)
    hest.insert(i);

  dag::cardinality_estimate card_est(hest);

  REQUIRE(hest.estimate() == card_est.estimate());
}
