#include <reticula/temporal_adjacency.hpp>

namespace reticula::adjacency {
auto simple::maximum_linger(VertexType) const -> double {
  return std::numeric_limits<double>::infinity();
}

limited_waiting_time::limited_waiting_time(double dt) : dt_(dt) {
  if (dt < 0.0)
    throw std::invalid_argument("Waiting time must be non-negative.");
}
auto limited_waiting_time::maximum_linger(VertexType) const -> double {
  return dt_;
}
auto limited_waiting_time::dt() const -> double { return dt_; }

exponential::exponential(double rate, std::size_t seed)
    : rate_(rate), seed_(seed) {
  if (rate <= 0.0)
    throw std::invalid_argument("Lambda must be positive.");
}
exponential::exponential(double rate)
    : exponential(rate, std::random_device{}()) {}
auto exponential::maximum_linger(VertexType) const -> double {
  return std::numeric_limits<double>::infinity();
}
auto exponential::rate() const -> double { return rate_; }
auto exponential::seed() const -> std::size_t { return seed_; }

geometric::geometric(double p, std::size_t seed) : p_(p), seed_(seed) {
  if (p <= 0.0 || p > 1.0)
    throw std::invalid_argument("Probability must be in (0, 1].");
}
geometric::geometric(double p) : geometric(p, std::random_device{}()) {}
auto geometric::maximum_linger(VertexType) const -> double {
  return std::numeric_limits<double>::infinity();
}
auto geometric::p() const -> double { return p_; }
auto geometric::seed() const -> std::size_t { return seed_; }
} // namespace reticula::adjacency
