#include "../../include/dag/generators.hpp"

namespace dag {
  template <
    integer_vertex VertT,
    std::uniform_random_bit_generator Gen>
  undirected_temporal_network<VertT, double>
  random_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator) {
    return random_activation_temporal_network<
      undirected_temporal_edge<VertT, double>>(
        complete_graph(size),
        max_t,
        [rate](const auto&) {
          return std::exponential_distribution<double>(rate); },
        [rate](const auto&) {
          return std::exponential_distribution<double>(rate); },
        generator,
        static_cast<std::size_t>(max_t*rate + 6*std::sqrt(max_t*rate)));
  }

  template <
    integer_vertex VertT,
    std::uniform_random_bit_generator Gen>
  directed_temporal_network<VertT, double>
  random_directed_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator) {
    return random_activation_temporal_network<
      directed_temporal_edge<VertT, double>>(
        complete_directed_graph(size),
        max_t,
        [rate](const auto&) {
          return std::exponential_distribution<double>(rate); },
        [rate](const auto&) {
          return std::exponential_distribution<double>(rate); },
        generator,
        static_cast<std::size_t>(max_t*rate + 6*std::sqrt(max_t*rate)));
  }

  template <
    temporal_edge EdgeT,
    typename ActivationF,
    typename ResActivationF,
    std::uniform_random_bit_generator Gen>
  requires
    random_number_distribution<std::invoke_result_t<
      ActivationF, typename EdgeT::StaticProjectionType>> &&
    random_number_distribution<std::invoke_result_t<
      ResActivationF, typename EdgeT::StaticProjectionType>> &&
    std::same_as<
      typename std::invoke_result_t<
        ResActivationF,
        typename EdgeT::StaticProjectionType>::result_type,
      typename std::invoke_result_t<
        ActivationF,
        typename EdgeT::StaticProjectionType>::result_type> &&
    std::constructible_from<
      EdgeT,
      typename EdgeT::StaticProjectionType,
      typename std::invoke_result_t<
        ActivationF,
        typename EdgeT::StaticProjectionType>::result_type>
  network<EdgeT>
  random_activation_temporal_network(
      network<typename EdgeT::StaticProjectionType> base_net,
      typename EdgeT::TimeType max_t,
      ActivationF&& inter_event_time_edge_activation,
      ResActivationF&& residual_time_edge_activation,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& e: base_net.edges()) {
      auto res_dist = residual_time_edge_activation(e);
      auto iet_dist = inter_event_time_edge_activation(e);
      for (auto t = res_dist(generator); t < max_t; t += iet_dist(generator))
        edges.emplace_back(e, t);
    }

    return network<EdgeT>(edges, base_net.vertices());
  }

  template <
    temporal_edge EdgeT,
    typename ActivationF,
    std::uniform_random_bit_generator Gen>
  requires
    random_number_distribution<std::invoke_result_t<
      ActivationF, typename EdgeT::StaticProjectionType>> &&
    std::constructible_from<
      EdgeT,
      typename EdgeT::StaticProjectionType,
      typename std::invoke_result_t<
        ActivationF,
        typename EdgeT::StaticProjectionType>::result_type>
  network<EdgeT>
  random_activation_temporal_network(
      network<typename EdgeT::StaticProjectionType> base_net,
      typename EdgeT::TimeType max_t,
      ActivationF&& inter_event_time_edge_activation,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& e: base_net.edges()) {
      auto iet_dist = inter_event_time_edge_activation(e);
      for (auto t = 0; t < max_t*2; t += iet_dist(generator))
        if (t >= max_t) edges.emplace_back(e, t - max_t);
    }

    return network<EdgeT>(edges, base_net.vertices());
  }
}  // namespace dag
