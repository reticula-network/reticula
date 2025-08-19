#pragma once

#include <cstdint>
#include <vector>

namespace reticula::combinadics {
auto capped_binomial_coefficient(
  std::uint64_t m, std::uint64_t k, std::uint64_t cap) -> std::uint64_t;

auto advance_by_skip(
  std::vector<std::uint64_t> v, std::uint64_t n, std::uint64_t skip)
  -> std::vector<std::uint64_t>;
} // namespace reticula::combinadics

namespace reticula::enumeration {

auto is_past_end(const std::vector<std::uint64_t>& v, std::uint64_t n) -> bool;

// v is a strictly-increasing d-tuple with 0 <= v[i] < n, or past-end.
// Lexicographic (row-major). All math in uint64_t. O(#carries + 1) per call.
auto advance_by_skip(
  std::vector<std::uint64_t> v, std::uint64_t n, std::uint64_t skip)
  -> std::vector<std::uint64_t>;
} // namespace reticula::enumeration
