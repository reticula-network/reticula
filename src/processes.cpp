#include <stdexcept>

#include <reticula/processes.hpp>

namespace reticula {
power_law::power_law(double exponent, double mean)
    : x_min_{mean * (exponent - 2.0) / (exponent - 1.0)}, exponent_(exponent),
      mean_(mean) {
  if (exponent <= 2)
    throw std::domain_error("exponent should be larger than 2");
  if (mean <= 0)
    throw std::domain_error("mean should be positive");
}

auto power_law::mean() const -> double { return mean_; }
auto power_law::exponent() const -> double { return exponent_; }
auto power_law::x_min() const -> double { return x_min_; }

residual_power_law::residual_power_law(double exponent, double mean)
    : x_min_{mean * (exponent - 2.0) / (exponent - 1.0)}, exponent_(exponent),
      mean_(mean) {
  if (exponent <= 2)
    throw std::domain_error("exponent should be larger than 2");
  if (mean <= 0)
    throw std::domain_error("mean should be positive");
}

auto residual_power_law::mean() const -> double { return mean_; }
auto residual_power_law::exponent() const -> double { return exponent_; }
auto residual_power_law::x_min() const -> double { return x_min_; }

hawkes_univariate_exponential::hawkes_univariate_exponential(
  double mu, double alpha, double theta, double phi)
    : mu_(mu), alpha_(alpha), theta_(theta), phi_(phi) {}

auto hawkes_univariate_exponential::mu() const -> double { return mu_; }
auto hawkes_univariate_exponential::alpha() const -> double { return alpha_; }
auto hawkes_univariate_exponential::theta() const -> double { return theta_; }
auto hawkes_univariate_exponential::phi() const -> double { return phi_; }

delta_distribution::delta_distribution(double mean) : mean_(mean) {}
auto delta_distribution::mean() const -> double { return mean_; }
} // namespace reticula
