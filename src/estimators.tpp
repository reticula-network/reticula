#include <cmath>

namespace dag {
  template <class T, class HyperLogLogT>
  hll_cardinality_estimator<T, HyperLogLogT>::hll_cardinality_estimator(
      std::size_t seed, std::size_t /* size_est */) :
      _hll(true, static_cast<uint32_t>(seed)) {}

  template <class T, class HyperLogLogT>
  double hll_cardinality_estimator<T, HyperLogLogT>::estimate() const {
    return _hll.estimate();
  }

  template <class T, class HyperLogLogT>
  void hll_cardinality_estimator<T, HyperLogLogT>::insert(const T& item) {
    _hll.insert(item);
  }


  template <class T, class HyperLogLogT>
  void hll_cardinality_estimator<T, HyperLogLogT>::merge(
      const hll_cardinality_estimator<T, HyperLogLogT>& other) {
    _hll.merge(other.underlying_estimator());
  }

  template <class T, class HyperLogLogT>
  const HyperLogLogT&
  hll_cardinality_estimator<T, HyperLogLogT>:: underlying_estimator() const {
    return _hll;
  }

  template <class T, class HyperLogLogT>
  double hll_cardinality_estimator<T, HyperLogLogT>::p_larger(
      double estimate, std::size_t limit, std::size_t max_size) {
    if (limit > max_size)
      return 0.0;

    constexpr double cutoff = 1e-10;

    double sigma = 1.05/pow(2.0, HyperLogLogT::dense_prec/2.0);

    double p_larger = 0.0;
    double p_total = 0.0;

    if (estimate < static_cast<double>(limit)*(1-sigma*6))
      return 0;

    auto normal_pdf = [](double x, double mean, double stddev) -> double {
      constexpr double inv_sqrt_2pi = 0.3989422804014327;
      double a = (x - mean)/stddev;

      return inv_sqrt_2pi/stddev*std::exp(-0.5*a*a);
    };

    double p_size = normal_pdf(
        estimate,
        static_cast<double>(limit),
        sigma*static_cast<double>(limit));

    if (p_size < cutoff) {
      if (estimate < static_cast<double>(limit))
        return 0;
      else
        return 1;
    }

    double min_search = (1 > 6*sigma) ? estimate - 6*sigma*estimate : 1;
    std::size_t i = static_cast<std::size_t>(min_search);
    while (i <= limit || (p_size > cutoff && i <= max_size)) {
      p_size = normal_pdf(
          estimate,
          static_cast<double>(i),
          sigma*static_cast<double>(i));
      p_total += p_size;
      if (i > limit)
        p_larger += p_size;
      i++;
    }

    return p_larger/p_total;
  }

  // unordered_set_cardinality_estimator

  template <class T>
  unordered_set_cardinality_estimator<T>::unordered_set_cardinality_estimator(
      std::size_t /* seed */, std::size_t size_est) {
    if (size_est > 0)
      _set.reserve(size_est);
  }

  template <class T>
  std::size_t
  unordered_set_cardinality_estimator<T>::size() const {
    return _set.size();
  }

  template <class T>
  double
  unordered_set_cardinality_estimator<T>::estimate() const {
    return static_cast<double>(_set.size());
  }

  template <class T>
  void unordered_set_cardinality_estimator<T>::insert(const T& item) {
    _set.insert(item);
  }

  template <class T>
  void unordered_set_cardinality_estimator<T>::merge(
      const unordered_set_cardinality_estimator<T>& other) {
    _set.insert(
        other.underlying_estimator().begin(),
        other.underlying_estimator().end());
  }

  template <class T>
  bool unordered_set_cardinality_estimator<T>::contains(const T& item) const {
    return _set.contains(item);
  }

  template <class T>
  inline const std::unordered_set<T, hash<T>>&
  unordered_set_cardinality_estimator<T>::underlying_estimator() const {
    return _set;
  }

  // cardinality_estimate
  template <cardinality_estimator EstimatorT>
  cardinality_estimate::cardinality_estimate(
      const EstimatorT& estimator) {
    _est = estimator.estimate();
  }

  inline double cardinality_estimate::estimate() const {
    return _est;
  }
}  // namespace dag
