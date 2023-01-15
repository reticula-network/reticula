#include <vector>
#include <functional>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
using Catch::Approx;

#include <reticula/ranges.hpp>
#include <reticula/stats.hpp>


// Up until we get a real zip_view in C++23
template <
  reticula::ranges::forward_range Range1,
  reticula::ranges::forward_range Range2>
std::vector<
  std::pair<
    reticula::ranges::range_value_t<Range1>,
    reticula::ranges::range_value_t<Range2>>>
zip(Range1&& r1, Range2&& r2) {
  std::vector<
    std::pair<
      reticula::ranges::range_value_t<Range1>,
      reticula::ranges::range_value_t<Range2>>> f;
  auto i1 = std::begin(r1);
  auto i2 = std::begin(r2);
  while (i1 != std::end(r1) && i2 != std::end(r2))
    f.emplace_back(*(i1++), *(i2++));
  return f;
}

TEST_CASE("pearson correlation coeafficient",
    "[reticula::pearson_correlation_coefficient]") {
  std::vector<double> x{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<double> big{
    100000000, 100000001, 100000002, 100000003, 100000004,
    100000005, 100000006, 100000007, 100000008, 100000009};
  std::vector<double> little{
    0.99999990, 0.99999991, 0.99999992, 0.99999993, 0.99999994,
    0.99999995, 0.99999996, 0.99999997, 0.99999998, 0.99999999};
  std::vector<double> huge{
    1e12, 2e12, 3e12, 4e12, 5e12,
    6e12, 7e12, 8e12, 9e12, 1e13};
  std::vector<double> tiny{
    1e-12, 2e-12, 3e-12, 4e-12, 5e-12,
    6e-12, 7e-12, 8e-12, 9e-12, 1e-11};

  REQUIRE(reticula::pearson_correlation_coefficient(zip(x, x)) ==
      Approx(1.0));

  SECTION("basic stability") {
    REQUIRE(reticula::pearson_correlation_coefficient(
          zip(x, x | reticula::views::transform(std::negate<double>{}))) ==
        Approx(-1.0));
    REQUIRE(reticula::pearson_correlation_coefficient(
          zip(x | reticula::views::transform(std::negate<double>{}), x)) ==
        Approx(-1.0));

    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(x, big)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(x, little)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(x, huge)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(x, tiny)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(big, big)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(big, little)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(big, huge)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(big, tiny)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(little, little)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(little, huge)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(huge, huge)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(huge, tiny)) ==
        Approx(1.0));
    REQUIRE(
        reticula::pearson_correlation_coefficient(zip(tiny, tiny)) ==
        Approx(1.0));
  }

  SECTION("basic value") {
    std::vector<std::pair<double, double>> f{{-1, 0}, {0, 0}, {1, 3}};
    REQUIRE(
        reticula::pearson_correlation_coefficient(f) ==
        Approx(std::sqrt(3.0)/2.0));
  }

  SECTION("constant variable") {
    std::vector<std::pair<double, double>> f{{0, 1}, {0, 0}, {0, 3}};
    REQUIRE(std::isnan(reticula::pearson_correlation_coefficient(f)));

    std::vector<std::pair<double, double>> f2(13, {9.0, 9.0});
    REQUIRE(std::isnan(reticula::pearson_correlation_coefficient(f2)));
  }

  SECTION("small vectors") {
    std::vector<std::pair<double, double>> f{{2.0, 1.0}};
    REQUIRE(std::isnan(reticula::pearson_correlation_coefficient(f)));
  }

  SECTION("empty vectors") {
    std::vector<std::pair<double, double>> f{};
    REQUIRE(std::isnan(reticula::pearson_correlation_coefficient(f)));
  }
}
