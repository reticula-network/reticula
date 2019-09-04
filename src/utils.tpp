#include <functional>

#define DAG_UTIL_GOLDEN_RATIO 0x9E3779B97F4A7C15

// Hashing utilities
namespace dag {
  namespace utils {
    template <typename T>
      inline size_t combine_hash(const size_t seed, const T& other) {
        return seed ^ (std::hash<T>{}(other) +
            DAG_UTIL_GOLDEN_RATIO + (seed<<6) + (seed>>2));
      }

    template <typename T1, typename T2>
      inline size_t unordered_hash(const T1& t1, const T2& t2) {
        size_t h1, h2;
        std::tie(h1, h2) = std::minmax(
            std::hash<T1>{}(t1), std::hash<T2>{}(t2));
        return h1 ^ (h2 + DAG_UTIL_GOLDEN_RATIO + (h1<<6) + (h1>>2));
      }
  }
}

