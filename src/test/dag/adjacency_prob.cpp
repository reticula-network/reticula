#include <cmath>
#include <iostream>

#include "catch.hpp"

#include "../../../include/dag/temporal_edges.hpp"
#include "../../../include/dag/adjacency_prob.hpp"

TEST_CASE("deterministic adjacency probability",
    "[dag::adjacency_prob::deterministic]") {
  using EdgeType = dag::directed_temporal_edge<int, int>;
  EdgeType a(1, 2, 3), b(2, 3, 4), c(2, 3, 5);
  dag::adjacency_prob::deterministic<EdgeType> prob1(2);
  dag::adjacency_prob::deterministic<EdgeType> prob2(3);

  REQUIRE(prob1.p(a, b) == 1.0);
  REQUIRE(prob2.p(a, b) == 1.0);

  REQUIRE(prob1.p(a, c) == 0.0);
  REQUIRE(prob2.p(a, c) == 1.0);

  REQUIRE(prob1.p(c, a) == 0.0);
}

TEST_CASE("exponential adjacency probability",
    "[dag::adjacency_prob::exponential]") {
  using EdgeType = dag::directed_temporal_edge<int, int>;
  EdgeType a(1, 2, 3), b(2, 3, 4), c(2, 3, 5);
  dag::adjacency_prob::exponential<EdgeType> prob1(1);
  dag::adjacency_prob::exponential<EdgeType> prob2(2);

  REQUIRE(prob1.p(a, b) == Approx(std::exp(-1.0/1.0)));
  REQUIRE(prob2.p(a, b) == Approx(1.0/2.0*std::exp(-1.0/2.0)));

  REQUIRE(prob1.p(a, c) == Approx(std::exp(-2.0/1.0)));
  REQUIRE(prob2.p(a, c) == Approx(1.0/2.0*std::exp(-2.0/2.0)));

  REQUIRE(prob1.p(c, a) == 0.0);
}

TEST_CASE("bernoulli trial function",
    "[dag::adjacency_prob::bernoulli_trial]") {
  using EdgeType = dag::directed_temporal_edge<int, int>;
  EdgeType a(1, 2, 3), b(2, 3, 4);
  SECTION("prob == 1.0 results in true answer") {
    REQUIRE(dag::adjacency_prob::bernoulli_trial(a, b, 1.0, 0ul));
  }

  SECTION("prob == 0.0 results in true answer") {
    REQUIRE_FALSE(dag::adjacency_prob::bernoulli_trial(a, b, 0.0, 0ul));
  }

  SECTION("performs bernoulli trials") {
    size_t total = 10000;
    for (double p: {0.2, 0.4, 0.6, 0.8}) {
      size_t success = 0;
      for (size_t seed = 0; seed < total; seed++)
        if(dag::adjacency_prob::bernoulli_trial(a, b, p, seed))
            success++;
      double ratio = static_cast<double>(success)/static_cast<double>(total);
      double sigma = std::sqrt(static_cast<double>(total)*p*(1-p));
      REQUIRE(ratio > p-3.0*sigma);
      REQUIRE(ratio < p+3.0*sigma);
    }
  }
}
