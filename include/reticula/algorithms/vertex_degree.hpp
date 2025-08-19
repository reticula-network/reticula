#pragma once

#include <cstddef>
#include <vector>

#include <reticula/concepts.hpp>

namespace reticula {
template <network_like NetT>
auto in_degree(const NetT& net, const VertexType& v) -> std::size_t;

template <network_like NetT>
auto out_degree(const NetT& net, const VertexType& v) -> std::size_t;

template <network_like NetT>
auto incident_degree(const NetT& net, const VertexType& v) -> std::size_t;

template <network_like NetT>
  requires is_undirected_v<NetT>
auto degree(const NetT& net, const VertexType& v) -> std::size_t;

template <network_like NetT>
auto in_degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
auto out_degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
auto incident_degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
  requires is_undirected_v<NetT>
auto degree_sequence(const NetT& net) -> std::vector<std::size_t>;

template <network_like NetT>
auto in_out_degree_pair_sequence(const NetT& net)
  -> std::vector<std::pair<std::size_t, std::size_t>>;
} // namespace reticula

namespace reticula {
template <network_like NetT>
auto in_degree(const NetT& net, const VertexType& v) -> std::size_t {
  return net.in_degree(v);
}

template <network_like NetT>
auto out_degree(const NetT& net, const VertexType& v) -> std::size_t {
  return net.out_degree(v);
}

template <network_like NetT>
auto incident_degree(const NetT& net, const VertexType& v) -> std::size_t {
  return net.degree(v);
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto degree(const NetT& net, const VertexType& v) -> std::size_t {
  return net.degree(v);
}

template <network_like NetT>
auto in_degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> res;
  res.reserve(net.vertices().size());

  for (const auto& v : net.vertices())
    res.push_back(net.in_degree(v));

  return res;
}

template <network_like NetT>
auto out_degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> res;
  res.reserve(net.vertices().size());

  for (const auto& v : net.vertices())
    res.push_back(net.out_degree(v));

  return res;
}

template <network_like NetT>
auto incident_degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> res;
  res.reserve(net.vertices().size());

  for (const auto& v : net.vertices())
    res.push_back(net.degree(v));

  return res;
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto degree_sequence(const NetT& net) -> std::vector<std::size_t> {
  std::vector<std::size_t> res;
  res.reserve(net.vertices().size());

  for (const auto& v : net.vertices())
    res.push_back(net.degree(v));

  return res;
}

template <network_like NetT>
auto in_out_degree_pair_sequence(const NetT& net)
  -> std::vector<std::pair<std::size_t, std::size_t>> {
  std::vector<std::pair<std::size_t, std::size_t>> res;
  res.reserve(net.vertices().size());

  for (const auto& v : net.vertices())
    res.emplace_back(net.in_degree(v), net.out_degree(v));

  return res;
}
} // namespace reticula
