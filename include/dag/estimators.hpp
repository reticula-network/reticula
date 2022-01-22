#ifndef INCLUDE_DAG_ESTIMATORS_HPP_
#define INCLUDE_DAG_ESTIMATORS_HPP_

#include "utils.hpp"

namespace dag {
  template <typename T>
  concept cardinality_estimator =
    requires(std::size_t seed, std::size_t size_est) {
      { T(seed, size_est) } -> std::convertible_to<T>;
    } && requires(const T& a) {
      { a.estimate() } -> std::convertible_to<double>;
      { a.underlying_estimator() } ->
        std::convertible_to<typename T::EstimatorType>;
    } && requires(T a, const T::ItemType& i) {
      a.insert(i);
    } && requires(T a, const T& b) {
      a.merge(b);
    };  // NOLINT(readability/braces)


  template <class T, class HyperLogLogT>
  class hll_cardinality_estimator {
  public:
    using ItemType =  T;
    using EstimatorType = HyperLogLogT;

    hll_cardinality_estimator(std::size_t seed, std::size_t size_est);

    double estimate() const;
    void insert(const T& item);
    void merge(const hll_cardinality_estimator<T, HyperLogLogT>& other);

    const EstimatorType& underlying_estimator() const;

    static double p_larger(double estimate, std::size_t limit,
        std::size_t max_size = std::numeric_limits<std::size_t>::max());
  private:
    EstimatorType _hll;
  };

  template <class T>
  class unordered_set_cardinality_estimator {
  public:
    using ItemType =  T;
    using EstimatorType = std::unordered_set<T, hash<T>>;

    unordered_set_cardinality_estimator(
        std::size_t seed, std::size_t size_est);

    double estimate() const;
    std::size_t size() const;

    void insert(const T& item);
    void merge(const unordered_set_cardinality_estimator<T>& other);

    bool contains(const T& item) const;
    const EstimatorType& underlying_estimator() const;

  private:
    EstimatorType _set;
  };

  class cardinality_estimate {
  public:
    template <cardinality_estimator EstimatorT>
    explicit cardinality_estimate(const EstimatorT& estimator);

    double estimate() const;
  private:
    double _est;
  };
}  // namespace dag

#include "../../src/estimators.tpp"

#endif  // INCLUDE_DAG_ESTIMATORS_HPP_
