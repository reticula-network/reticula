#pragma once

#include <algorithm>
#include <random>
#include <ranges>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {}) -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto connected_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {}) -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto topology_constrained_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {}) -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto degree_constrained_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {},
  std::size_t iters = 5) -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto connected_degree_constrained_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {},
  std::size_t iters = 5) -> network<typename NetT::EdgeType>;
} // namespace reticula

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <reticula/algorithms/properties.hpp>
#include <reticula/algorithms/static_reachability.hpp>
#include <reticula/operations/addition.hpp>
#include <reticula/operations/projections.hpp>
#include <reticula/operations/subgraph.hpp>

namespace reticula {
namespace detail {
template <static_network_edge EdgeT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<EdgeT>
auto shuffling_mapping(const network<EdgeT>& proj, Gen& generator)
  -> std::vector<EdgeT> {
  std::unordered_set<EdgeT> new_links;

  auto nodes = proj.vertices();
  std::uniform_int_distribution<std::size_t> d(0, nodes.size() - 1);
  while (new_links.size() < proj.edges().size()) {
    std::size_t i = d(generator), j = d(generator);
    while (i == j)
      j = d(generator);
    new_links.emplace(nodes[i], nodes[j]);
  }

  std::vector<EdgeT> new_links_v(new_links.begin(), new_links.end());
  std::ranges::shuffle(new_links_v, generator);
  return new_links_v;
}

template <static_network_edge EdgeT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<EdgeT>
auto curveball_mapping(
  const network<EdgeT>& proj, Gen& generator, std::size_t iters)
  -> std::vector<EdgeT> {
  auto nodes = proj.vertices();
  if (nodes.size() < 2)
    return {};

  std::unordered_map<VertexType, std::vector<VertexType>> successors;
  successors.reserve(nodes.size());
  for (auto v : nodes) {
    successors[v] = proj.successors(v);
    std::ranges::sort(successors[v]);
  }

  std::vector<VertexType> new_v, new_w;

  std::uniform_int_distribution<std::size_t> d(0, nodes.size() - 1);
  std::size_t trades = iters * nodes.size();
  for (std::size_t trade = 0; trade < trades; trade++) {
    std::size_t vid = d(generator), wid = d(generator);
    while (vid == wid)
      wid = d(generator);
    VertexType v = nodes[vid];
    VertexType w = nodes[wid];

    const auto& sv = successors.at(v);
    const auto& sw = successors.at(w);

    std::size_t k = 0;
    std::size_t m = 0;
    {
      auto i = sv.begin(), j = sw.begin();
      while (i != sv.end() || j != sw.end()) {
        if (j == sw.end() || (i != sv.end() && *i < *j)) {
          if (*i != w)
            ++k;
          ++i;
        } else if (i == sv.end() || *j < *i) {
          if (*j != v)
            ++m;
          ++j;
        } else {
          ++i;
          ++j;
        }
      }
    }
    if (k + m == 0)
      continue;

    new_v.clear();
    new_v.reserve(sv.size());

    new_w.clear();
    new_w.reserve(sw.size());

    std::size_t need_v = k;
    std::size_t remaining = k + m;
    std::uniform_int_distribution<std::size_t> coin;
    using param_type = decltype(coin)::param_type;
    auto i = sv.begin(), j = sw.begin();
    while (i != sv.end() || j != sw.end()) {
      if (j == sw.end() || (i != sv.end() && *i < *j)) {
        // the v-w link
        if (*i == w) {
          new_v.push_back(w);
          ++i;
          continue;
        }
        // exclusive to v
        std::size_t r = coin(generator, param_type{0, remaining - 1});
        if (r < need_v) {
          new_v.push_back(*i);
          --need_v;
        } else {
          new_w.push_back(*i);
        }
        --remaining;
        ++i;
      } else if (i == sv.end() || *j < *i) {
        // the v-w link
        if (*j == v) {
          new_w.push_back(v);
          ++j;
          continue;
        }

        // exclusive to w
        std::size_t r = coin(generator, param_type{0, remaining - 1});
        if (r < need_v) {
          new_v.push_back(*j);
          --need_v;
        } else {
          new_w.push_back(*j);
        }
        --remaining;
        ++j;
      } else {
        // common to both
        new_v.push_back(*i);
        new_w.push_back(*i);
        ++i;
        ++j;
      }
    }

    std::vector<VertexType> added_v, added_w, removed_v, removed_w;
    if constexpr (is_undirected_v<EdgeT>) {
      added_v.reserve(new_v.size());
      std::ranges::set_difference(new_v, sv, std::back_inserter(added_v));

      added_w.reserve(new_w.size());
      std::ranges::set_difference(new_w, sw, std::back_inserter(added_w));

      removed_v.reserve(sv.size());
      std::ranges::set_difference(sv, new_v, std::back_inserter(removed_v));

      removed_w.reserve(sw.size());
      std::ranges::set_difference(sw, new_w, std::back_inserter(removed_w));
    }

    auto& out_v = successors.at(v);
    auto& out_w = successors.at(w);
    out_v.assign(new_v.begin(), new_v.end());
    out_w.assign(new_w.begin(), new_w.end());

    if constexpr (is_undirected_v<EdgeT>) {
      for (auto u : added_v) {
        auto& succs = successors.at(u);
        auto it = std::ranges::lower_bound(succs, v);
        if (it == succs.end() || *it != v)
          succs.insert(it, v);
      }
      for (auto u : added_w) {
        auto& succs = successors.at(u);
        auto it = std::ranges::lower_bound(succs, w);
        if (it == succs.end() || *it != w)
          succs.insert(it, w);
      }
      for (auto u : removed_v) {
        auto& succs = successors.at(u);
        auto it = std::ranges::lower_bound(succs, v);
        if (it != succs.end() && *it == v)
          succs.erase(it);
      }
      for (auto u : removed_w) {
        auto& succs = successors.at(u);
        auto it = std::ranges::lower_bound(succs, w);
        if (it != succs.end() && *it == w)
          succs.erase(it);
      }
    }
  }

  std::vector<EdgeT> res;
  std::size_t total = 0;
  for (auto& [v, succs] : successors)
    total += succs.size();
  res.reserve(total);
  for (auto& [v, succs] : successors)
    for (auto s : succs)
      if (is_directed_v<EdgeT> || v < s)
        res.emplace_back(v, s);
  std::ranges::shuffle(res, generator);
  return res;
}
} // namespace detail

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links) -> network<typename NetT::EdgeType> {
  using EdgeT = NetT::EdgeType;
  using LinkT = EdgeT::StaticProjectionType;

  auto proj = with_edges(static_projection(net), unobserved_links);
  auto new_links = detail::shuffling_mapping(proj, generator);

  auto events = std::views::zip_transform(
                  [&net](const LinkT& from, const LinkT& to) {
                    return link_timeline(net, from) |
                           std::views::transform([to](const EdgeT& e) -> EdgeT {
                             if constexpr (is_instantaneous_v<EdgeT>)
                               return {to, e.cause_time()};
                             else
                               return {to, e.cause_time(), e.effect_time()};
                           });
                  },
                  proj.edges(), new_links) |
                std::views::join;

  return network<EdgeT>(events, proj.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto connected_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links) -> network<typename NetT::EdgeType> {
  using EdgeT = NetT::EdgeType;
  using LinkT = EdgeT::StaticProjectionType;

  auto proj = with_edges(static_projection(net), unobserved_links);
  auto comps = weakly_connected_components(proj);
  auto new_links =
    comps | std::views::transform([&proj, &generator](const auto& c) {
      auto e =
        detail::shuffling_mapping(vertex_induced_subgraph(proj, c), generator);
      while (!is_weakly_connected(network<LinkT>(e, c))) {
        e = detail::shuffling_mapping(
          vertex_induced_subgraph(proj, c), generator);
      }
      return e;
    }) |
    std::views::join;
  auto old_links = comps | std::views::transform([&proj](const auto& c) {
                     auto sub = vertex_induced_subgraph(proj, c);
                     auto e = sub.edges();
                     return std::vector<LinkT>{e.begin(), e.end()};
                   }) |
                   std::views::join;

  auto events = std::views::zip_transform(
                  [&net](const LinkT& from, const LinkT& to) {
                    return link_timeline(net, from) |
                           std::views::transform([to](const EdgeT& e) -> EdgeT {
                             if constexpr (is_instantaneous_v<EdgeT>)
                               return {to, e.cause_time()};
                             else
                               return {to, e.cause_time(), e.effect_time()};
                           });
                  },
                  old_links, new_links) |
                std::views::join;

  return network<EdgeT>(events, proj.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto topology_constrained_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links) -> network<typename NetT::EdgeType> {
  using EdgeT = NetT::EdgeType;
  using LinkT = EdgeT::StaticProjectionType;

  auto proj = with_edges(static_projection(net), unobserved_links);

  auto old_links = proj.edges();
  std::vector<LinkT> new_links_v(old_links.begin(), old_links.end());
  std::ranges::shuffle(new_links_v, generator);

  auto events = std::views::zip_transform(
                  [&net](const LinkT& from, const LinkT& to) {
                    return link_timeline(net, from) |
                           std::views::transform([to](const EdgeT& e) -> EdgeT {
                             if constexpr (is_instantaneous_v<EdgeT>)
                               return {to, e.cause_time()};
                             else
                               return {to, e.cause_time(), e.effect_time()};
                           });
                  },
                  old_links, new_links_v) |
                std::views::join;
  return network<EdgeT>(events, proj.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto degree_constrained_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links,
  std::size_t iters) -> network<typename NetT::EdgeType> {
  using EdgeT = NetT::EdgeType;
  using LinkT = EdgeT::StaticProjectionType;

  auto proj = with_edges(static_projection(net), unobserved_links);
  auto new_links = detail::curveball_mapping(proj, generator, iters);

  auto events = std::views::zip_transform(
                  [&net](const LinkT& from, const LinkT& to) {
                    return link_timeline(net, from) |
                           std::views::transform([to](const EdgeT& e) -> EdgeT {
                             if constexpr (is_instantaneous_v<EdgeT>)
                               return {to, e.cause_time()};
                             else
                               return {to, e.cause_time(), e.effect_time()};
                           });
                  },
                  proj.edges(), new_links) |
                std::views::join;

  return network<EdgeT>(events, proj.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto connected_degree_constrained_link_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links,
  std::size_t iters) -> network<typename NetT::EdgeType> {
  using EdgeT = NetT::EdgeType;
  using LinkT = EdgeT::StaticProjectionType;

  auto proj = with_edges(static_projection(net), unobserved_links);
  auto comps = weakly_connected_components(proj);
  auto new_links =
    comps | std::views::transform([&proj, &generator, iters](const auto& c) {
      auto e = detail::curveball_mapping(
        vertex_induced_subgraph(proj, c), generator, iters);
      while (!is_weakly_connected(network<LinkT>(e, c)))
        e = detail::curveball_mapping(
          vertex_induced_subgraph(proj, c), generator, iters);
      return e;
    }) |
    std::views::join;
  auto old_links = comps | std::views::transform([&proj](const auto& c) {
                     auto sub = vertex_induced_subgraph(proj, c);
                     auto e = sub.edges();
                     return std::vector<LinkT>{e.begin(), e.end()};
                   }) |
                   std::views::join;

  auto events = std::views::zip_transform(
                  [&net](const LinkT& from, const LinkT& to) {
                    return link_timeline(net, from) |
                           std::views::transform([to](const EdgeT& e) -> EdgeT {
                             if constexpr (is_instantaneous_v<EdgeT>)
                               return {to, e.cause_time()};
                             else
                               return {to, e.cause_time(), e.effect_time()};
                           });
                  },
                  old_links, new_links) |
                std::views::join;

  return network<EdgeT>(events, proj.vertices());
}
} // namespace reticula
