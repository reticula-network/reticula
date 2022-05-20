#include <catch2/catch.hpp>

#include <reticula/distributions.hpp>


TEST_CASE("power-law with secified mean",
    "[reticula::power_law_with_specified_mean]") {
  STATIC_REQUIRE(reticula::random_number_distribution<
      reticula::power_law_with_specified_mean<>>);
}

TEST_CASE("residual power-law with specified mean",
    "[reticula::residual_power_law_with_specified_mean]") {
  STATIC_REQUIRE(reticula::random_number_distribution<
      reticula::residual_power_law_with_specified_mean<>>);
}

TEST_CASE("hawkes univariate exponential",
    "[reticula::hawkes_univariate_exponential]") {
  STATIC_REQUIRE(reticula::random_number_distribution<
      reticula::hawkes_univariate_exponential<>>);
}

TEST_CASE("delta distribution",
    "[reticula::delta_distribution]") {
  STATIC_REQUIRE(reticula::random_number_distribution<
      reticula::delta_distribution<int>>);
  STATIC_REQUIRE(reticula::random_number_distribution<
      reticula::delta_distribution<double>>);
}
