#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/temporal_adjacency.hpp>
#include <reticula/temporal_clusters.hpp>
#include <reticula/views/enumerate.hpp>

namespace reticula {

// Single source

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster(const NetT& net, VertexType v, double time, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster(const NetT& net, typename NetT::EdgeType e, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster(const NetT& net, VertexType v, double time, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster(const NetT& net, typename NetT::EdgeType e, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_cluster(const NetT& net, VertexType v, double time, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_cluster(const NetT& net, typename NetT::EdgeType e, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType>;

// All sources

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_clusters(const NetT& net, const AdjT& adj) -> std::vector<std::pair<
  typename NetT::EdgeType, temporal_cluster<typename NetT::EdgeType>>>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_clusters(const NetT& net, const AdjT& adj) -> std::vector<std::pair<
  typename NetT::EdgeType, temporal_cluster<typename NetT::EdgeType>>>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_clusters(const NetT& net, const AdjT& adj, bool singletons = true)
  -> std::vector<temporal_cluster<typename NetT::EdgeType>>;

// All source sizes

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster_sizes(const NetT& net, const AdjT& adj)
  -> std::vector<std::pair<typename NetT::EdgeType, temporal_cluster_size>>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster_sizes(const NetT& net, const AdjT& adj)
  -> std::vector<std::pair<typename NetT::EdgeType, temporal_cluster_size>>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_cluster_sizes(
  const NetT& net, const AdjT& adj, bool singletons = true)
  -> std::vector<temporal_cluster_size>;

// All source size estimates

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster_size_estimates(
  const NetT& net, const AdjT& adj, double temporal_resolution,
  std::size_t seed)
  -> std::vector<
    std::pair<typename NetT::EdgeType, temporal_cluster_size_estimate>>;

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster_size_estimates(
  const NetT& net, const AdjT& adj, double temporal_resolution,
  std::size_t seed)
  -> std::vector<
    std::pair<typename NetT::EdgeType, temporal_cluster_size_estimate>>;
} // namespace reticula

#include <queue>

#include <ds/disjoint_set.hpp>

#include <reticula/event_graphs.hpp>

namespace reticula {
namespace detail {
template <temporal_network_edge EdgeT>
struct make_loop_t {
  auto operator()(VertexType v, double time) const -> EdgeT {
    if constexpr (is_instantaneous_v<EdgeT>)
      return EdgeT(v, v, time);
    else
      return EdgeT(v, v, time, time);
  }
};

template <temporal_network_edge EdgeT>
  requires(!is_dyadic_v<EdgeT>)
struct make_loop_t<EdgeT> {
  auto operator()(VertexType v, double time) const -> EdgeT {
    if constexpr (is_instantaneous_v<EdgeT>)
      return EdgeT({v}, {v}, time);
    else
      return EdgeT({v}, {v}, time, time);
  }
};

template <temporal_network_edge EdgeT>
auto make_loop(VertexType v, double time) -> EdgeT {
  return make_loop_t<EdgeT>()(v, time);
}

enum struct cluster_direction { out, in };

template <
  cluster_direction Dir, temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto generic_cluster(
  const NetT& net, const AdjT& adj, const typename NetT::EdgeType& root)
  -> temporal_cluster<typename NetT::EdgeType> {
  temporal_cluster<typename NetT::EdgeType> res;
  res.insert(root, adj);

  using EdgeT = typename NetT::EdgeType;

  std::queue<EdgeT> search({root});

  while (!search.empty()) {
    EdgeT e = search.front();
    search.pop();

    std::vector<EdgeT> next;
    if constexpr (Dir == cluster_direction::out)
      next = event_graph::successors(net, e, adj, true);
    else
      next = event_graph::predecessors(net, e, adj, true);

    for (auto&& s : next)
      if (!res.contains(s)) {
        search.push(s);
        res.insert(s, adj);
      }
  }

  return res;
}

template <
  cluster_direction Dir, temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT,
  cluster_like<AdjT> InterimT, std::constructible_from<InterimT> Res>
auto generic_clusters(
  const NetT& net, const AdjT& adj, double temporal_resolution,
  std::size_t seed) -> std::vector<std::pair<typename NetT::EdgeType, Res>> {
  using EdgeT = typename NetT::EdgeType;
  std::unordered_map<EdgeT, InterimT> ongoing_clusters;
  std::unordered_map<EdgeT, std::size_t> in_counts;

  std::vector<std::pair<EdgeT, Res>> res;
  res.reserve(net.edges_cause().size());

  // TODO: use non-uniqued successors/predecessors based off of
  // vertex_successors etc for both functions. This avoids copies.
  auto in_edges_size = [&](const EdgeT& e) {
    if constexpr (Dir == cluster_direction::in)
      return event_graph::successors(net, e, adj, true).size();
    else
      return event_graph::predecessors(net, e, adj, true).size();
  };
  auto out_edges = [&](const EdgeT& e) {
    if constexpr (Dir == cluster_direction::in)
      return event_graph::predecessors(net, e, adj, true);
    else
      return event_graph::successors(net, e, adj, true);
  };

  auto order = [&] {
    if constexpr (Dir == cluster_direction::out)
      return net.edges_cause() | std::views::reverse;
    else
      return net.edges_effect() | std::views::all;
  }();

  auto make_cluster = [&] -> InterimT {
    if constexpr (std::same_as<
                    InterimT, temporal_cluster_sketch<typename NetT::EdgeType>>)
      return {temporal_resolution, seed};
    else
      return {};
  };

  for (auto& edge : order) {
    auto cluster = make_cluster();
    cluster.insert(edge, adj);
    ongoing_clusters.emplace(edge, std::move(cluster));

    in_counts.emplace(edge, in_edges_size(edge));

    for (auto& other : out_edges(edge)) {
      ongoing_clusters.at(edge).merge(ongoing_clusters.at(other));
      in_counts.at(other)--;

      if (in_counts.at(other) == 0) {
        res.emplace_back(other, std::move(ongoing_clusters.at(other)));
        in_counts.erase(other);
        ongoing_clusters.erase(other);
      }
    }

    if (in_counts.at(edge) == 0) {
      res.emplace_back(edge, std::move(ongoing_clusters.at(edge)));
      in_counts.erase(edge);
      ongoing_clusters.erase(edge);
    }
  }

  return res;
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT,
  std::constructible_from<temporal_cluster<typename NetT::EdgeType>> Res>
auto generic_weak_clusters(const NetT& net, const AdjT& adj, bool singletons)
  -> std::vector<Res> {
  auto disj_set = ds::disjoint_set<std::size_t>(net.edges().size());

  std::unordered_map<typename NetT::EdgeType, std::size_t> edge_id;
  edge_id.reserve(net.edges().size());
  for (auto&& [i, e] : net.edges() | views::enumerate)
    edge_id.emplace(e, i);

  auto events = net.edges_cause();
  auto iter = events.begin();

  while (iter != events.end()) {
    auto iter_idx =
      static_cast<std::size_t>(std::distance(events.begin(), iter));

    for (auto&& other : event_graph::successors(net, *iter, adj, true))
      disj_set.merge(iter_idx, edge_id.at(other));
    ++iter;
  }

  auto sets = disj_set.sets(singletons);
  std::vector<Res> res;
  res.reserve(sets.size());
  for (const auto& [idx, set] : sets) {
    temporal_cluster<typename NetT::EdgeType> current_set(set.size());
    for (const auto& event_idx : set)
      current_set.insert(events[event_idx], adj);

    res.emplace_back(std::move(current_set));
  }

  return res;
}
} // namespace detail

// single source

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster(const NetT& net, VertexType v, double time, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType> {
  return out_cluster(
    net, detail::make_loop<typename NetT::EdgeType>(v, time), adj);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster(const NetT& net, typename NetT::EdgeType e, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType> {
  return detail::generic_cluster<detail::cluster_direction::out, NetT>(
    net, adj, e);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster(const NetT& net, VertexType v, double time, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType> {
  return in_cluster(
    net, detail::make_loop<typename NetT::EdgeType>(v, time), adj);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster(const NetT& net, typename NetT::EdgeType e, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType> {
  return detail::generic_cluster<detail::cluster_direction::in, NetT>(
    net, adj, e);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_cluster(const NetT& net, VertexType v, double time, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType> {
  return weak_cluster(
    net, detail::make_loop<typename NetT::EdgeType>(v, time), adj);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_cluster(const NetT& net, typename NetT::EdgeType e, const AdjT& adj)
  -> temporal_cluster<typename NetT::EdgeType> {
  temporal_cluster<typename NetT::EdgeType> res;
  res.insert(e, adj);

  using EdgeT = typename NetT::EdgeType;

  std::queue<EdgeT> search({e});

  while (!search.empty()) {
    EdgeT e = search.front();
    search.pop();

    for (auto&& s : event_graph::successors(net, e, adj, true))
      if (!res.contains(s)) {
        search.push(s);
        res.insert(s, adj);
      }
    for (auto&& s : event_graph::predecessors(net, e, adj, true))
      if (!res.contains(s)) {
        search.push(s);
        res.insert(s, adj);
      }
  }

  return res;
}

// all sources

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_clusters(const NetT& net, const AdjT& adj) -> std::vector<std::pair<
  typename NetT::EdgeType, temporal_cluster<typename NetT::EdgeType>>> {
  return detail::generic_clusters<
    detail::cluster_direction::out, NetT, AdjT,
    temporal_cluster<typename NetT::EdgeType>,
    temporal_cluster<typename NetT::EdgeType>>(net, adj, 0.0, 0);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_clusters(const NetT& net, const AdjT& adj) -> std::vector<std::pair<
  typename NetT::EdgeType, temporal_cluster<typename NetT::EdgeType>>> {
  return detail::generic_clusters<
    detail::cluster_direction::in, NetT, AdjT,
    temporal_cluster<typename NetT::EdgeType>,
    temporal_cluster<typename NetT::EdgeType>>(net, adj, 0.0, 0);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_clusters(const NetT& net, const AdjT& adj, bool singletons)
  -> std::vector<temporal_cluster<typename NetT::EdgeType>> {
  return detail::generic_weak_clusters<
    NetT, AdjT, temporal_cluster<typename NetT::EdgeType>>(
    net, adj, singletons);
}

// all source sizes

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster_sizes(const NetT& net, const AdjT& adj)
  -> std::vector<std::pair<typename NetT::EdgeType, temporal_cluster_size>> {
  return detail::generic_clusters<
    detail::cluster_direction::out, NetT, AdjT,
    temporal_cluster<typename NetT::EdgeType>, temporal_cluster_size>(
    net, adj, 0.0, 0);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster_sizes(const NetT& net, const AdjT& adj)
  -> std::vector<std::pair<typename NetT::EdgeType, temporal_cluster_size>> {
  return detail::generic_clusters<
    detail::cluster_direction::in, NetT, AdjT,
    temporal_cluster<typename NetT::EdgeType>, temporal_cluster_size>(
    net, adj, 0.0, 0);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto weak_cluster_sizes(const NetT& net, const AdjT& adj, bool singletons)
  -> std::vector<temporal_cluster_size> {
  return detail::generic_weak_clusters<NetT, AdjT, temporal_cluster_size>(
    net, adj, singletons);
}

// all source size estimates

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto out_cluster_size_estimates(
  const NetT& net, const AdjT& adj, double temporal_resolution,
  std::size_t seed)
  -> std::vector<
    std::pair<typename NetT::EdgeType, temporal_cluster_size_estimate>> {
  return detail::generic_clusters<
    detail::cluster_direction::out, NetT, AdjT,
    temporal_cluster_sketch<typename NetT::EdgeType>,
    temporal_cluster_size_estimate>(net, adj, temporal_resolution, seed);
}

template <
  temporal_network_like NetT,
  adjacency::adjacency<typename NetT::EdgeType> AdjT>
auto in_cluster_size_estimates(
  const NetT& net, const AdjT& adj, double temporal_resolution,
  std::size_t seed)
  -> std::vector<
    std::pair<typename NetT::EdgeType, temporal_cluster_size_estimate>> {
  return detail::generic_clusters<
    detail::cluster_direction::in, NetT, AdjT,
    temporal_cluster_sketch<typename NetT::EdgeType>,
    temporal_cluster_size_estimate>(net, adj, temporal_resolution, seed);
}
} // namespace reticula
