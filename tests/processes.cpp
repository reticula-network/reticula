#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/processes.hpp>

TEST_CASE(
  "power-law with specified mean",
  "[reticula::power_law_with_specified_mean]") {
  STATIC_REQUIRE(reticula::point_process<reticula::power_law, double>);
}

TEST_CASE(
  "residual power-law with specified mean",
  "[reticula::residual_power_law_with_specified_mean]") {
  STATIC_REQUIRE(reticula::point_process<reticula::residual_power_law, double>);
}

TEST_CASE(
  "hawkes univariate exponential",
  "[reticula::hawkes_univariate_exponential]") {
  STATIC_REQUIRE(
    reticula::point_process<reticula::hawkes_univariate_exponential, double>);
}

TEST_CASE("delta distribution", "[reticula::delta_distribution]") {
  STATIC_REQUIRE(reticula::point_process<reticula::delta_distribution, double>);
}
