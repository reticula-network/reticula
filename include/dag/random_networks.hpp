#ifndef INCLUDE_DAG_RANDOM_NETWORKS_HPP_
#define INCLUDE_DAG_RANDOM_NETWORKS_HPP_

#include <random>

namespace dag {
  template <typename VertT>
  undirected_network<VertT> gnp_random_graph(size_t n, double p,
      std::mt19937_64& generator);

  template <typename VertT>
  undirected_network<VertT> ba_random_graph(size_t n, size_t m,
      std::mt19937_64& generator);

  template <class EdgeT, class Distribution, class ResDistribution>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      ResDistribution residual_time_dist,
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
