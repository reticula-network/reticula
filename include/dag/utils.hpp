#ifndef INCLUDE_DAG_UTILS_HPP_
#define INCLUDE_DAG_UTILS_HPP_

#include <string>
#include <functional>

#define DAG_UTIL_GOLDEN_RATIO 0x9E3779B97F4A7C15

namespace dag {
  /**
    A collection of utility functions and classes.
   */
  namespace utils {
    /**
      Calculates hash of `other` and combines it with a seed.

      @code{.cpp}
      std::pair<int, int> p(12, 13);
      size_t p_hash =
        dag::utils::combine_hash(std::hash<int>{}(p.first), p.second);
      @endcode

      @param seed Seed or hash or another 
      @param other Value that would be hashed and mixed with `seed` 
     */
    template <class T, template<typename> typename Hash = std::hash>
    inline size_t combine_hash(const size_t seed, const T& other) {
      return seed ^ (Hash<T>{}(other) +
          DAG_UTIL_GOLDEN_RATIO + (seed << 6) + (seed >> 2));
    }


    /**
      Calculates hash of two things regardless of the order of inputs.

      @code{.cpp}
      assert(dag::utils::unordered_hash(12, 13) ==
              dag::utils::unordered_hash(13, 12));
      @endcode
     */
    template <class T1, class T2, template<typename> typename Hash = std::hash>
    inline size_t unordered_hash(const T1& t1, const T2& t2) {
      size_t h1, h2;
      std::tie(h1, h2) = std::minmax(
          Hash<T1>{}(t1), Hash<T2>{}(t2));
      return h1 ^ (h2 + DAG_UTIL_GOLDEN_RATIO + (h1 << 6) + (h1 >> 2));
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
}  // namespace dag

#endif  // INCLUDE_DAG_UTILS_HPP_
