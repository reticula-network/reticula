#include <concepts>
#include <stdexcept>

namespace reticula {
  // power_law_with_specified_mean
  template <std::floating_point RealType>
  power_law_with_specified_mean<RealType>::power_law_with_specified_mean(
      RealType exponent, RealType mean) : _exponent(exponent), _mean(mean) {
    if (exponent <= 2)
      throw std::domain_error("exponent should be larger than 2");
    if (mean <= 0)
      throw std::domain_error("mean should be positive");

    _x_min = _mean*(_exponent - 2.0)/(_exponent - 1.0);
  }

  template <std::floating_point RealType>
  template <std::uniform_random_bit_generator Generator>
  RealType
  power_law_with_specified_mean<RealType>::operator()(Generator& g) const {
    std::uniform_real_distribution<RealType> dist;
    return _x_min*std::pow((1.0 - dist(g)), 1.0/(1.0 - _exponent));
  }

  template <std::floating_point RealType>
  RealType power_law_with_specified_mean<RealType>::x_min() const {
    return _x_min;
  }

  template <std::floating_point RealType>
  RealType power_law_with_specified_mean<RealType>::exponent() const {
    return _exponent;
  }

  template <std::floating_point RealType>
  RealType power_law_with_specified_mean<RealType>::mean() const {
    return _mean;
  }


  // residual_power_law_with_specified_mean
  template <std::floating_point RealType>
  residual_power_law_with_specified_mean<RealType>::
    residual_power_law_with_specified_mean(RealType exponent, RealType mean)
      : _exponent(exponent), _mean(mean) {
    if (exponent <= 2)
      throw std::domain_error("exponent should be larger than 2");
    if (mean <= 0)
      throw std::domain_error("mean should be positive");

    _x_min = _mean*(_exponent - 2.0)/(_exponent - 1.0);
  }

  template <std::floating_point RealType>
  template <std::uniform_random_bit_generator Generator>
  RealType residual_power_law_with_specified_mean<RealType>::operator()(
      Generator& g) const {
    auto u = std::uniform_real_distribution<RealType>{}(g);
    if (u*_mean < _x_min)
      return u*_mean;

    return _x_min * std::pow(
      (1.0-u) * (_exponent-1.0), -1.0/(_exponent-2.0));
  }

  template <std::floating_point RealType>
  RealType residual_power_law_with_specified_mean<RealType>::x_min() const {
    return _x_min;
  }

  template <std::floating_point RealType>
  RealType residual_power_law_with_specified_mean<RealType>::exponent() const {
    return _exponent;
  }

  template <std::floating_point RealType>
  RealType residual_power_law_with_specified_mean<RealType>::mean() const {
    return _mean;
  }

  // Hawkes_univariate_exponential
  template <std::floating_point RealType>
  hawkes_univariate_exponential<RealType>::hawkes_univariate_exponential(
    RealType mu, RealType alpha, RealType theta, RealType phi) :
      _mu(mu), _alpha(alpha), _theta(theta), _phi(phi) {}

  // The implementation here is based on Ogata’s modified thinning algorithm.
  // Inspired by https://git.io/JD2cm
  template <std::floating_point RealType>
  template <std::uniform_random_bit_generator Generator>
  RealType hawkes_univariate_exponential<RealType>::operator()(Generator& g) {
    RealType iet = {}, d = {}, ed, m, lda, r2;
    std::uniform_real_distribution dist;
    do {
      m = _mu + _alpha * _theta * (static_cast<RealType>(1.0) + _phi);

      RealType e = -std::log(dist(g))/m;
      iet += e;

      ed = std::exp(-_theta * (e + d));
      lda = _mu + _alpha * _theta * ed * (static_cast<RealType>(1.0) + _phi);

      r2 = dist(g);
      d += e;
    } while (r2 * m > lda);

    _phi = ed * (1.0 + _phi);
    return iet;
  }

  template <std::floating_point RealType>
  RealType hawkes_univariate_exponential<RealType>::mu() const {
    return _mu;
  }

  template <std::floating_point RealType>
  RealType hawkes_univariate_exponential<RealType>::alpha() const {
    return _alpha;
  }

  template <std::floating_point RealType>
  RealType hawkes_univariate_exponential<RealType>::theta() const {
    return _theta;
  }

  template <std::floating_point RealType>
  RealType hawkes_univariate_exponential<RealType>::phi() const {
    return _phi;
  }

  // delta_distribution
  template <typename ResultType>
  requires std::integral<ResultType> || std::floating_point<ResultType>
  delta_distribution<ResultType>::delta_distribution(ResultType mean):
    _mean(mean) {}

  template <typename ResultType>
  requires std::integral<ResultType> || std::floating_point<ResultType>
  template <std::uniform_random_bit_generator Generator>
  ResultType delta_distribution<ResultType>::operator()(Generator& /* g */) {
    return _mean;
  }

  template <typename ResultType>
  requires std::integral<ResultType> || std::floating_point<ResultType>
  ResultType delta_distribution<ResultType>::mean() const {
    return _mean;
  }
}  // namespace reticula
