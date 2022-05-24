#include "../../include/reticula/generators.hpp"

namespace reticula {
  template <
    integer_vertex VertT,
    std::uniform_random_bit_generator Gen>
  undirected_temporal_network<VertT, double>
  random_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator) {
    return random_link_activation_temporal_network<
      undirected_temporal_edge<VertT, double>>(
        complete_graph(size),
        max_t,
        std::exponential_distribution<double>(rate),
        std::exponential_distribution<double>(rate),
        generator,
        static_cast<std::size_t>(max_t*rate + 6*std::sqrt(max_t*rate)));
  }

  template <
    integer_vertex VertT,
    std::uniform_random_bit_generator Gen>
  directed_temporal_network<VertT, double>
  random_directed_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator) {
    return random_link_activation_temporal_network<
      directed_temporal_edge<VertT, double>>(
        complete_directed_graph(size),
        max_t,
        std::exponential_distribution<double>(rate),
        std::exponential_distribution<double>(rate),
        generator,
        static_cast<std::size_t>(max_t*rate + 6*std::sqrt(max_t*rate)));
  }

  template <
    temporal_edge EdgeT,
    random_number_distribution IETDist,
    random_number_distribution ResDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::same_as<
      typename IETDist::result_type, typename ResDist::result_type> &&
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_link_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      ResDist res_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& e: base_net.edges())
      for (auto t = res_dist(generator); t < max_t; t += iet_dist(generator))
        edges.emplace_back(e, t);

    return network<EdgeT>(edges, base_net.vertices());
  }

  template <
    temporal_edge EdgeT,
    random_number_distribution IETDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_link_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& e: base_net.edges())
      for (typename EdgeT::TimeType t{}; t < max_t*2; t += iet_dist(generator))
        if (t >= max_t) edges.emplace_back(e, t - max_t);

    return network<EdgeT>(edges, base_net.vertices());
  }
}  // namespace reticula
