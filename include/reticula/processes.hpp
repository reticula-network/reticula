#pragma once

#include <random>

namespace reticula {

class power_law {
public:
  power_law(double exponent, double mean);

  template <std::uniform_random_bit_generator Generator>
  [[nodiscard]] auto operator()(Generator& g) const -> double;

  [[nodiscard]] auto x_min() const -> double;
  [[nodiscard]] auto exponent() const -> double;
  [[nodiscard]] auto mean() const -> double;

private:
  double x_min_, exponent_, mean_;
};

class residual_power_law {
public:
  residual_power_law(double exponent, double mean);

  template <std::uniform_random_bit_generator Generator>
  [[nodiscard]] auto operator()(Generator& g) const -> double;

  [[nodiscard]] auto x_min() const -> double;
  [[nodiscard]] auto exponent() const -> double;
  [[nodiscard]] auto mean() const -> double;

private:
  double x_min_, exponent_, mean_;
};

class hawkes_univariate_exponential {
public:
  using result_type = double;

  hawkes_univariate_exponential(
    double mu, double alpha, double theta, double phi = 0.0);

  template <std::uniform_random_bit_generator Generator>
  [[nodiscard]] auto operator()(Generator& g) -> double;

  [[nodiscard]] auto mu() const -> double;
  [[nodiscard]] auto alpha() const -> double;
  [[nodiscard]] auto theta() const -> double;
  [[nodiscard]] auto phi() const -> double;

private:
  double mu_, alpha_, theta_, phi_;
};

class delta_distribution {
public:
  explicit delta_distribution(double mean);

  template <std::uniform_random_bit_generator Generator>
  [[nodiscard]] auto operator()(Generator& g) const -> double;

  [[nodiscard]] auto mean() const -> double;

private:
  double mean_;
};
} // namespace reticula

namespace reticula {
template <std::uniform_random_bit_generator Generator>
auto power_law::operator()(Generator& g) const -> double {
  auto u = std::uniform_real_distribution<double>{}(g);
  return x_min_ * std::pow((1.0 - u), 1.0 / (1.0 - exponent_));
}

template <std::uniform_random_bit_generator Generator>
auto residual_power_law::operator()(Generator& g) const -> double {
  auto u = std::uniform_real_distribution<double>{}(g);
  if (u * mean_ < x_min_)
    return u * mean_;

  return x_min_ *
         std::pow((1.0 - u) * (exponent_ - 1.0), -1.0 / (exponent_ - 2.0));
}

// The implementation here is based on Ogata’s modified thinning algorithm.
// Inspired by https://git.io/JD2cm
template <std::uniform_random_bit_generator Generator>
auto hawkes_univariate_exponential::operator()(Generator& g) -> double {
  double iet = 0.0, d = 0.0, ed = 0.0, m = 0.0, lda = 0.0, r2 = 0.0;
  std::uniform_real_distribution dist;
  do {
    m = mu_ + alpha_ * theta_ * (1.0 + phi_);

    double e = -std::log(dist(g)) / m;
    iet += e;

    ed = std::exp(-theta_ * (e + d));
    lda = mu_ + alpha_ * theta_ * ed * (1.0 + phi_);

    r2 = dist(g);
    d += e;
  } while (r2 * m > lda);

  phi_ = ed * (1.0 + phi_);
  return iet;
}

template <std::uniform_random_bit_generator Generator>
auto delta_distribution::operator()(Generator&) const -> double {
  return mean_;
}
} // namespace reticula
