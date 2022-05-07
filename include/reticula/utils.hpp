#ifndef INCLUDE_RETICULA_UTILS_HPP_
#define INCLUDE_RETICULA_UTILS_HPP_

#include <string>
#include <functional>
#include <stdexcept>

#include <hll/hyperloglog.hpp>

#include "network_concepts.hpp"

#define RETICULA_UTIL_GOLDEN_RATIO 0x9E3779B97F4A7C15

namespace reticula {
  /**
    A collection of utility functions and classes.
   */
  namespace utils {
    /**
      Calculates hash of `other` and combines it with a seed.

      @code{.cpp}
      std::pair<int, int> p(12, 13);
      size_t p_hash =
        reticula::utils::combine_hash(std::hash<int>{}(p.first), p.second);
      @endcode

      @param seed Seed or hash or another 
      @param other Value that would be hashed and mixed with `seed` 
     */
    template <class T, template<typename> class HashStruct>
    inline size_t combine_hash(const size_t seed, const T& other) {
      return seed ^ (HashStruct<T>{}(other) +
          RETICULA_UTIL_GOLDEN_RATIO + (seed << 6) + (seed >> 2));
    }


    /**
      Calculates hash of two things regardless of the order of inputs.

      @code{.cpp}
      assert(reticula::utils::unordered_hash(12, 13) ==
              reticula::utils::unordered_hash(13, 12));
      @endcode
     */
    template <class T1, class T2, template<typename> class HashStruct>
    inline size_t unordered_hash(const T1& t1, const T2& t2) {
      size_t h1, h2;
      std::tie(h1, h2) = std::minmax(
          HashStruct<T1>{}(t1), HashStruct<T2>{}(t2));
      return h1 ^ (h2 + RETICULA_UTIL_GOLDEN_RATIO + (h1 << 6) + (h1 >> 2));
    }

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
      Indicates that the specified vertex type cannot label required number of
      unique vertices.
     */
    class vertex_type_too_small_error : public std::invalid_argument {
    public:
      explicit vertex_type_too_small_error(const std::string& what_arg)
        : std::invalid_argument(what_arg) {}
      explicit vertex_type_too_small_error(const char* what_arg)
        : std::invalid_argument(what_arg) {}
    };
  }  // namespace utils

  template <typename Key>
  struct hash {
    std::size_t operator()(const Key& k) const noexcept;
  };

  // If std::hash has an implementation, defer to that
  template <hashable_with<std::hash> Key>
  struct hash<Key> {
    std::size_t operator()(const Key& k) const noexcept {
      return std::hash<Key>{}(k);
    }
  };

  template <typename T1, typename T2>
  struct hash<std::pair<T1, T2>> {
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
      return utils::combine_hash<T2, reticula::hash>(
          reticula::hash<T1>{}(p.first), p.second);
    }
  };
}  // namespace reticula

namespace hll {
  template <typename T1, typename T2>
  struct hash<std::pair<T1, T2>> {
    std::size_t operator()(
        const std::pair<T1, T2>& p, uint32_t seed) const noexcept {
      return hll::hash<size_t>{}(
          reticula::hash<std::pair<T1, T2>>{}(p), seed);
    }
  };
}  // namespace hll

#endif  // INCLUDE_RETICULA_UTILS_HPP_
