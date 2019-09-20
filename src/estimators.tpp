#include <cmath>

namespace dag {
  template <class T, class HyperLogLogT>
  double hll_estimator<T, HyperLogLogT>::p_larger(
      double estimate, size_t limit, size_t max_size) {
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
    size_t i = static_cast<size_t>(min_search);
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
}  // namespace dag
