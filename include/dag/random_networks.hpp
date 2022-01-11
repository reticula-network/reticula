#ifndef INCLUDE_DAG_RANDOM_NETWORKS_HPP_
#define INCLUDE_DAG_RANDOM_NETWORKS_HPP_

#include <random>
#include <concepts>
#include <numeric>

#include "network_concepts.hpp"

namespace dag {
  template <typename T>
  concept random_number_distribution =
    std::is_arithmetic_v<typename T::result_type> &&
    requires(T dist, std::mt19937_64& gen) {
      { dist(gen) } -> std::convertible_to<typename T::result_type>;
    };  // NOLINT(readability/braces)

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> gnp_random_graph(
      VertT n, double p,
      Gen& generator);

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> ba_random_graph(
      VertT n, VertT m,
      Gen& generator);

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> random_regular_graph(
      VertT size, VertT degree,
      Gen& generator);


  /**
    Random events with the given `inter_event_time_dist` and
    `residual_time_dist` for generating time to first event.
    */
  template <
    temporal_edge EdgeT,
    random_number_distribution Distribution,
    random_number_distribution ResDistribution,
    std::uniform_random_bit_generator Gen>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      ResDistribution residual_time_dist,
      Gen& generator,
      std::size_t size_hint = 0);

  /**
    Random events with the given inter-event time distribution and `max_t`
    units of time burn-in before being recorded.
    */
  template <
    temporal_edge EdgeT,
    random_number_distribution Distribution,
    std::uniform_random_bit_generator Gen>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      Gen& generator,
      std::size_t size_hint = 0);


  template <class RealType = double>
  class power_law_with_specified_mean {
  public:
    using result_type = RealType;

    power_law_with_specified_mean(RealType exponent, RealType mean);

    template <class Generator>
    RealType operator()(Generator& g) const;

    RealType x_min() const;
    RealType exponent() const;
    RealType mean() const;
  private:
    RealType _x_min, _exponent, _mean;
  };

  template <class RealType = double>
  class residual_power_law_with_specified_mean {
  public:
    using result_type = RealType;

    residual_power_law_with_specified_mean(RealType exponent, RealType mean);

    template <class Generator>
    RealType operator()(Generator& g) const;

    RealType x_min() const;
    RealType exponent() const;
    RealType mean() const;
  private:
    RealType _x_min, _exponent, _mean;
  };

  template <class RealType = double>
  class hawkes_univariate_exponential {
  public:
    using result_type = RealType;

    hawkes_univariate_exponential(
        RealType mu, RealType alpha, RealType theta, RealType phi = {});

    template <class Generator>
    RealType operator()(Generator& g);

    RealType mu() const;
    RealType alpha() const;
    RealType theta() const;
    RealType phi() const;
  private:
    RealType _mu, _alpha, _theta, _phi;
  };
}  // namespace dag

#include "../../src/random_networks.tpp"

#endif  // INCLUDE_DAG_RANDOM_NETWORKS_HPP_
