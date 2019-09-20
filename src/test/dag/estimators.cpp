#include <unordered_set>

#include "catch.hpp"

#include "../../../include/dag/estimators.hpp"

TEST_CASE("hll_estimators", "[dag::temporal_component]") {
  // TODO
}

TEST_CASE("hll_estimator_readonly", "[dag::temporal_component]") {
  // TODO
}

TEST_CASE("exact_estimator", "[dag::temporal_component]") {
  dag::exact_estimator<int> est(0ul, 0);
  est.insert(1);
  est.insert(2);

  SECTION("basic properties") {
    REQUIRE(est.size() == 2);
    REQUIRE(est.contains(1));
    REQUIRE_FALSE(est.contains(3));
    REQUIRE(est.set() == std::unordered_set<int>(1, 2));
  }

  SECTION("insertion") {
    est.insert(3);
    REQUIRE(est.size() == 3);
    REQUIRE(est.contains(3));
    REQUIRE(est.set() == std::unordered_set<int>(1, 2, 3));
  }

  SECTION("merging") {
    dag::exact_estimator<int> est2(0ul, 0);
    est2.insert(3);

    est.merge(est2);

    REQUIRE(est.size() == 3);
    REQUIRE(est.contains(3));
    REQUIRE(est.set() == std::unordered_set<int>(1, 2, 3));
  }
}
