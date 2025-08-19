#include <cstdint>
#include <stdexcept>
#include <vector>

#include <reticula/enumeration.hpp>

auto reticula::enumeration::is_past_end(
  const std::vector<std::uint64_t>& v, std::uint64_t n) -> bool {
  return !v.empty() && v.back() >= n;
}

auto reticula::enumeration::advance_by_skip(
  std::vector<std::uint64_t> v, std::uint64_t n, std::uint64_t skip)
  -> std::vector<std::uint64_t> {
  const std::size_t d = v.size();
  if (d == 0)
    throw std::invalid_argument("vector must not be empty");

  if (d > n || is_past_end(v, n)) {
    v.back() = n;
    return v;
  }

  const auto n_minus_d = n - static_cast<std::uint64_t>(d);
  const auto last_max = n - 1;

  while (skip && !is_past_end(v, n)) {
    const std::uint64_t room =
      last_max - v[d - 1];
    const std::uint64_t take = std::min(skip, room);
    v[d - 1] += take;
    skip -= take;
    if (skip == 0)
      break;

    std::size_t pos = d;
    for (std::size_t j = d; j-- > 0;) {
      const std::uint64_t max_j = n_minus_d + static_cast<std::uint64_t>(j);
      if (v[j] < max_j) {
        pos = j;
        break;
      }
    }

    if (pos == d) {
      v.back() = n;
      break;
    }

    --skip;
    ++v[pos];
    for (std::size_t j = pos + 1; j < d; ++j)
      v[j] = v[j - 1] + 1;
  }

  return v;
}
