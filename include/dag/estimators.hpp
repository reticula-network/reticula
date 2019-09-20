#ifndef INCLUDE_DAG_ESTIMATORS_HPP_
#define INCLUDE_DAG_ESTIMATORS_HPP_

namespace dag {
  template <class T, class HyperLogLogT>
  class hll_estimator {
  public:
    hll_estimator(size_t seed, size_t /*size_est*/) :
      _estimator(true, seed) {}

    double estimate() const { return _estimator.estimate(); }
    void insert(const T& item) { _estimator.insert(item); }
    void merge(const hll_estimator<T, HyperLogLogT>& other) {
      _estimator.merge(other.estimator());
    }

    const HyperLogLogT& estimator() const { return _estimator; }

    static double p_larger(double estimate, size_t limit,
        size_t max_size = std::numeric_limits<std::size_t>::max());
  private:
    HyperLogLogT _estimator;
  };

  template <class T, class HyperLogLogT>
  class hll_estimator_readonly {
  public:
    hll_estimator_readonly(size_t /*seed*/, size_t size_est)
      : _est(static_cast<double>(size_est)) {}

   explicit hll_estimator_readonly(
       const hll_estimator<T, HyperLogLogT>& hll_est) {
      _est = hll_est.estimate();
    }

    double estimate() const { return _est; }
    void insert(const T& item) {
      throw std::logic_error("cannot insert into read-only hll estimator");
    }
    void merge(const hll_estimator_readonly<T, HyperLogLogT>& other) {
      throw std::logic_error("cannot merge read-only hll estimator");
    }

    static double p_larger(double estimate, size_t limit,
        size_t max_size = std::numeric_limits<std::size_t>::max()) {
      return hll_estimator<T, HyperLogLogT>::p_larger(
          estimate, limit, max_size);
    }

  private:
    double _est;
  };

  template <class T>
  class exact_estimator {
  public:
    exact_estimator(size_t /*seed*/, size_t size_est) {
      if (size_est > 0)
        _set.reserve(size_est);
    }

    size_t size() const { return _set.size(); }
    void insert(const T& item) { _set.insert(item); }
    void merge(const exact_estimator<T>& other) {
      _set.insert(other.set().begin(), other.set().end());
    }
    bool contains(const T& item) const { return _set.find(item) != _set.end(); }
    const std::unordered_set<T>& set() const { return _set; }

  private:
    std::unordered_set<T> _set;
  };
}  // namespace dag

#include "../../src/estimators.tpp"

#endif  // INCLUDE_DAG_ESTIMATORS_HPP_
