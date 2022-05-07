#ifndef INCLUDE_RETICULA_INTERVALS_HPP_
#define INCLUDE_RETICULA_INTERVALS_HPP_

#include <vector>
#include <utility>

namespace reticula {
  template <typename T>
  class interval_set {
  public:
    using ValueType = T;
    using IteratorType = typename std::vector<std::pair<T, T>>::const_iterator;

    interval_set() = default;

    void insert(T start, T end);
    void merge(const interval_set<T>& cs);

    bool covers(T time) const;
    T cover() const;

    IteratorType begin() const;
    IteratorType end() const;

    bool operator==(const interval_set<T>& other) const = default;
  private:
    std::vector<std::pair<T, T>> _ints;
    bool can_merge(std::pair<T, T> a, std::pair<T, T> b) const;
  };
}  // namespace reticula

#include "../../src/intervals.tpp"

#endif  // INCLUDE_RETICULA_INTERVALS_HPP_
