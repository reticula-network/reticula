#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto instant_event_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType>;
}

namespace reticula {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto instant_event_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType> {
  using EdgeT = typename NetT::EdgeType;
  using StaticEdgeT = typename EdgeT::StaticProjectionType;
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto nodes = net.vertices();

  std::vector<EdgeT> shuffled;
  shuffled.reserve(net.edges_cause().size());

  std::uniform_int_distribution<std::size_t> d(0, nodes.size() - 1);
  auto sample_verts = [&d](Gen& gen) {
    std::size_t i = d(gen), j = d(gen);
    while (i == j)
      j = d(gen);
    return std::make_pair(i, j);
  };

  std::unordered_set<EdgeT, hash<EdgeT>> current_set;
  current_set.reserve(32);

  auto build_event = [](
                       const typename EdgeT::StaticProjectionType& sp,
                       const EdgeT& ev) -> EdgeT {
    if constexpr (is_instantaneous_v<EdgeT>)
      return EdgeT(sp, ev.cause_time());
    else
      return EdgeT(sp, ev.cause_time(), ev.effect_time());
  };

  auto last_cause_time = net.edges_cause().front().cause_time();
  for (auto& event : net.edges_cause()) {
    if (event.cause_time() > last_cause_time) {
      shuffled.insert(shuffled.end(), current_set.begin(), current_set.end());
      current_set.clear();
      last_cause_time = event.cause_time();
    }

    auto [i, j] = sample_verts(generator);
    StaticEdgeT static_edge{i, j};
    auto new_event = build_event(static_edge, event);
    while (current_set.contains(new_event)) {
      auto [i, j] = sample_verts(generator);
      StaticEdgeT static_edge{i, j};
      new_event = build_event(static_edge, event);
    }
    current_set.insert(new_event);
  }

  shuffled.insert(shuffled.end(), current_set.begin(), current_set.end());
  return network<EdgeT>(shuffled, nodes);
}
} // namespace reticula
