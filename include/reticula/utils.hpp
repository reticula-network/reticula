#pragma once

#include <cstddef>
#include <format>
#include <utility>

#include <reticula/concepts.hpp>

namespace std {
template <typename>
struct hash;
}
namespace hll {
template <typename>
struct hash;
}

/**
  A collection of utility functions and classes.
 */
namespace reticula::utils {

template <class T>
constexpr auto minmax(const T& a, const T& b) -> std::pair<const T&, const T&> {
  return (b < a) ? std::pair<const T&, const T&>(b, a)
                 : std::pair<const T&, const T&>(a, b);
}

template <class T>
constexpr auto minmax_array(const T& a, const T& b) -> std::array<T, 2> {
  return (b < a) ? std::array<T, 2>{b, a} : std::array<T, 2>{a, b};
}

constexpr std::size_t golden_ratio = 0x9E3779B97F4A7C15;

/**
  Calculates hash of `other` and combines it with a seed.

  @code{.cpp}
  std::pair<int, int> p(12, 13);
  std::size_t p_hash =
    reticula::utils::combine_hash(std::hash<int>{}(p.first), p.second);
  @endcode

  @param seed Seed or hash or another
  @param other Value that would be hashed and mixed with `seed`
 */
template <class T, template <typename> class HashStruct>
inline auto combine_hash(const std::size_t seed, const T& other)
  -> std::size_t {
  return seed ^
         (HashStruct<T>{}(other) + golden_ratio + (seed << 6) + (seed >> 2));
}

/**
  Calculates hash of two things regardless of the order of inputs.

  @code{.cpp}
  assert(reticula::utils::unordered_hash(12, 13) ==
          reticula::utils::unordered_hash(13, 12));
  @endcode
 */
template <class T1, class T2, template <typename> class HashStruct>
inline auto unordered_hash(const T1& t1, const T2& t2) -> std::size_t {
  auto [h1, h2] = minmax(HashStruct<T1>{}(t1), HashStruct<T2>{}(t2));
  return h1 ^ (h2 + golden_ratio + (h1 << 6) + (h1 >> 2));
}

template <std::ranges::input_range R>
auto join(const R& range, const std::string& delimiter = ", ") -> std::string {
  std::string result;
  for (const auto& item : range) {
    if (!result.empty())
      result += delimiter;
    result += std::format("{}", item);
  }
  return result;
}

template <
  std::forward_iterator I1, std::sentinel_for<I1> S1, std::forward_iterator I2,
  std::sentinel_for<I2> S2, class Comp = std::ranges::less>
constexpr auto
intersects(I1 first1, S1 last1, I2 first2, S2 last2, Comp comp = {}) -> bool {
  while (first1 != last1 && first2 != last2) {
    if (!comp(*first1, *first2) && !comp(*first2, *first1))
      return true;

    comp(*first1, *first2) ? ++first1 : ++first2;
  }
  return false;
}

template <
  std::ranges::forward_range R1, std::ranges::forward_range R2,
  class Comp = std::ranges::less>
constexpr auto intersects(R1&& r1, R2&& r2, Comp comp = {}) -> bool {
  return intersects(
    std::ranges::begin(r1), std::ranges::end(r1), std::ranges::begin(r2),
    std::ranges::end(r2), comp);
}
} // namespace reticula::utils

namespace reticula {
template <typename T>
struct edge_relabel {
  using EdgeType = T;
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const EdgeType& e, MapF&& map_fun) const -> EdgeType;
};

template <typename Key>
struct hash {
  auto operator()(const Key& k) const noexcept -> std::size_t;
};

// If std::hash has an implementation, defer to that
template <hashable_with<std::hash> Key>
struct hash<Key> {
  auto operator()(const Key& k) const noexcept -> std::size_t {
    return std::hash<Key>{}(k);
  }
};

template <typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
  auto operator()(const std::pair<T1, T2>& p) const noexcept -> std::size_t {
    return utils::combine_hash<T2, reticula::hash>(
      reticula::hash<T1>{}(p.first), p.second);
  }
};

/**
  Indicates that the a network containing cycles was passed to a function
  only defined for acyclic graphs.
 */
class not_acyclic_error : public std::domain_error {
public:
  explicit not_acyclic_error(const std::string& what_arg)
      : std::domain_error(what_arg) {}
  explicit not_acyclic_error(const char* what_arg)
      : std::domain_error(what_arg) {}
};

/**
  Indicates that the a network a non-bipartite network was passed to a function
  only defined for bipartite graphs.
 */
class not_bipartite_error : public std::domain_error {
public:
  explicit not_bipartite_error(const std::string& what_arg)
      : std::domain_error(what_arg) {}
  explicit not_bipartite_error(const char* what_arg)
      : std::domain_error(what_arg) {}
};

/**
  Indicates that the a network containing disconnected components or no vertices
  was passed to a function estimating or calculating distances.
 */
class distance_undefined_error : public std::domain_error {
public:
  explicit distance_undefined_error(const std::string& what_arg)
      : std::domain_error(what_arg) {}
  explicit distance_undefined_error(const char* what_arg)
      : std::domain_error(what_arg) {}
};
} // namespace reticula
