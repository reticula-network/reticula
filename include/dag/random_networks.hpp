#ifndef INCLUDE_DAG_RANDOM_NETWORKS_HPP_
#define INCLUDE_DAG_RANDOM_NETWORKS_HPP_

#include <random>

#include "network_concepts.hpp"

namespace dag {
  template <network_vertex VertT>
  undirected_network<VertT> gnp_random_graph(VertT n, double p,
      std::mt19937_64& generator);

  template <network_vertex VertT>
  undirected_network<VertT> ba_random_graph(VertT n, VertT m,
      std::mt19937_64& generator);

  /**
    Random events with the given `inter_event_time_dist` and `residual_time_dist`
    for generating time to first event.
    */
  template <temporal_edge EdgeT, class Distribution, class ResDistribution>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      ResDistribution residual_time_dist,
      size_t seed,
      size_t size_hint = 0);

  /**
    Random events with the given inter-event time distribution and `max_t`
    units of time burn-in before being recorded.
    */
  template <temporal_edge EdgeT, class Distribution>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      size_t seed,
      size_t size_hint = 0);

  template<class RealType = double>
    class truncated_power_law_distribution {
    public:
      RealType exponent, x0, x1, constant, mean;
      std::uniform_real_distribution<RealType> dist;

      truncated_power_law_distribution(
          RealType exponent,
          RealType x0,
          RealType x1,
          RealType average);

      template<class Generator>
        RealType operator()(Generator& g);
    };
}  // namespace dag

#include "../../src/random_networks.tpp"

#endif  // INCLUDE_DAG_RANDOM_NETWORKS_HPP_
