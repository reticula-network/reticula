#pragma once

#include <cmath>
#include <limits>

#include <reticula/concepts.hpp>

namespace reticula {
template <typename R, typename U, typename V>
concept attribute_pairlike_range =
  std::ranges::input_range<R> &&
  requires {
    typename std::tuple_size<
      std::remove_cvref_t<std::ranges::range_reference_t<R>>>;
  } &&
  std::convertible_to<
    std::tuple_element_t<
      0, std::remove_cvref_t<std::ranges::range_reference_t<R>>>,
    U> &&
  std::convertible_to<
    std::tuple_element_t<
      1, std::remove_cvref_t<std::ranges::range_reference_t<R>>>,
    V>;

/**
  Calculates Pearson's correlation coefficient of the two variables in the
  vector f.
*/
auto pearson_correlation_coefficient(
  attribute_pairlike_range<double, double> auto&& attrs) -> double;
} // namespace reticula

namespace reticula {
auto pearson_correlation_coefficient(
  attribute_pairlike_range<double, double> auto&& attrs) -> double {
  std::size_t n = 0;
  double mx = 0.0, my = 0.0;
  double sx = 0.0, sy = 0.0, cov = 0.0;

  for (auto [x, y] : attrs) {
    ++n;
    double dx = x - mx;
    mx += dx / static_cast<double>(n);
    double dy = y - my;
    my += dy / static_cast<double>(n);

    sx += dx * (x - mx);
    sy += dy * (y - my);
    cov += dx * (y - my);
  }

  if (n < 2 || sx == 0.0 || sy == 0.0)
    return std::numeric_limits<double>::quiet_NaN();

  return cov / std::sqrt(sx * sy);
}
} // namespace reticula
