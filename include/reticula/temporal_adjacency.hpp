#pragma once

#include <reticula/concepts.hpp>
#include <reticula/utils.hpp>

namespace reticula::adjacency {
template <typename T, typename EdgeT>
concept adjacency = temporal_network_edge<EdgeT> &&
                    requires(const T& t, const EdgeT& e, VertexType v) {
                      { t.linger(e, v) } -> std::convertible_to<double>;
                      { t.maximum_linger(v) } -> std::convertible_to<double>;
                    };

struct simple {
  template <temporal_network_edge EdgeT>
  [[nodiscard]] auto linger(const EdgeT& e, VertexType v) const -> double;

  [[nodiscard]] auto maximum_linger(VertexType v) const -> double;
};

struct limited_waiting_time {
  limited_waiting_time(double dt);

  template <temporal_network_edge EdgeT>
  [[nodiscard]] auto linger(const EdgeT& a, VertexType b) const -> double;

  [[nodiscard]] auto maximum_linger(VertexType v) const -> double;

  [[nodiscard]] auto dt() const -> double;

private:
  double dt_;
};

struct exponential {
  exponential(double rate);
  exponential(double rate, std::size_t seed);

  template <temporal_network_edge EdgeT>
  [[nodiscard]] auto linger(const EdgeT& a, VertexType v) const -> double;
  [[nodiscard]] auto maximum_linger(VertexType v) const -> double;

  [[nodiscard]] auto seed() const -> std::size_t;

  [[nodiscard]] auto rate() const -> double;

private:
  double rate_;
  std::size_t seed_;
};

struct geometric {
  geometric(double p);
  geometric(double p, std::size_t seed);

  template <temporal_network_edge EdgeT>
  [[nodiscard]] auto linger(const EdgeT& a, VertexType v) const -> double;
  [[nodiscard]] auto maximum_linger(VertexType v) const -> double;

  [[nodiscard]] auto seed() const -> std::size_t;

  [[nodiscard]] auto p() const -> double;

private:
  double p_;
  std::size_t seed_;
};
} // namespace reticula::adjacency

#include <limits>
#include <random>

namespace reticula::adjacency {
template <temporal_network_edge EdgeT>
auto simple::linger(const EdgeT&, VertexType) const -> double {
  return std::numeric_limits<double>::infinity();
}

template <temporal_network_edge EdgeT>
auto limited_waiting_time::linger(const EdgeT&, VertexType) const -> double {
  return dt_;
}

template <temporal_network_edge EdgeT>
auto exponential::linger(const EdgeT& a, VertexType v) const -> double {
  auto deterministic_seed = utils::combine_hash<VertexType, std::hash>(
    utils::combine_hash<EdgeT, std::hash>(seed_, a), v);
  std::mt19937_64 gen(deterministic_seed);
  return std::exponential_distribution<double>{rate_}(gen);
}

template <temporal_network_edge EdgeT>
auto geometric::linger(const EdgeT& a, VertexType v) const -> double {
  auto deterministic_seed = utils::combine_hash<VertexType, std::hash>(
    utils::combine_hash<EdgeT, std::hash>(seed_, a), v);
  std::mt19937_64 gen(deterministic_seed);
  return static_cast<double>(
    std::geometric_distribution<std::uint64_t>{p_}(gen));
}
} // namespace reticula::adjacency
