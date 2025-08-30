#pragma once

#include <functional>
#include <reticula/concepts.hpp>
#include <reticula/stats.hpp>
#include <vector>

namespace reticula {

template <static_network_like NetT, std::invocable<VertexType> AttrFun>
  requires is_undirected_v<NetT>
[[nodiscard]] auto attribute_assortativity(const NetT& net, AttrFun&& attr_fun)
  -> double;

template <static_network_like NetT, mapping<VertexType, double> AttrMap>
  requires is_undirected_v<NetT>
[[nodiscard]] auto attribute_assortativity(
  const NetT& net, const AttrMap& attr_map, double default_value) -> double;

template <
  static_network_like NetT, std::invocable<VertexType> AttrFun1,
  std::invocable<VertexType> AttrFun2>
[[nodiscard]] auto attribute_assortativity(
  const NetT& net, AttrFun1&& mutator_attribute_fun,
  AttrFun2&& mutated_attribute_fun) -> double;

template <
  static_network_like NetT, mapping<VertexType, double> MutatorAttrMap,
  mapping<VertexType, double> MutatedAttrMap>
[[nodiscard]] auto attribute_assortativity(
  const NetT& net, const MutatorAttrMap& mutator_attr_map,
  const MutatedAttrMap& mutated_attr_map, double mutator_default_value,
  double mutated_default_value) -> double;

template <static_network_like NetT>
  requires is_undirected_v<NetT>
[[nodiscard]] auto degree_assortativity(const NetT& net) -> double;

enum struct direction { in, out };

template <static_network_like NetT>
[[nodiscard]] auto
degree_assortativity(const NetT& net, direction tail, direction head) -> double;
} // namespace reticula

namespace reticula {
template <static_network_like NetT, std::invocable<VertexType> AttrFun>
  requires is_undirected_v<NetT>
auto attribute_assortativity(const NetT& net, AttrFun&& attr_fun) -> double {
  std::vector<std::pair<double, double>> vals;
  vals.reserve(net.edges().size());
  for (auto& e : net.edges())
    for (auto& i : e.mutator_verts())
      for (auto& j : e.mutated_verts())
        if (i != j)
          vals.emplace_back(attr_fun(i), attr_fun(j));

  return pearson_correlation_coefficient(vals);
}

template <static_network_like NetT, mapping<VertexType, double> AttrMap>
  requires is_undirected_v<NetT>
auto attribute_assortativity(
  const NetT& net, const AttrMap& attr_map, double default_value) -> double {
  return attribute_assortativity(
    net, [&attr_map, default_value](VertexType v) -> double {
      auto it = attr_map.find(v);
      if (it == attr_map.end())
        return default_value;
      else
        return it->second;
    });
}

template <
  static_network_like NetT, std::invocable<VertexType> AttrFun1,
  std::invocable<VertexType> AttrFun2>
auto attribute_assortativity(
  const NetT& net, AttrFun1&& mutator_attribute_fun,
  AttrFun2&& mutated_attribute_fun) -> double {
  std::vector<std::pair<double, double>> vals;
  vals.reserve(net.edges().size());
  for (auto& e : net.edges())
    for (auto& i : e.mutator_verts())
      for (auto& j : e.mutated_verts())
        vals.emplace_back(mutator_attribute_fun(i), mutated_attribute_fun(j));
  return pearson_correlation_coefficient(vals);
}

template <
  static_network_like NetT, mapping<VertexType, double> MutatorAttrMap,
  mapping<VertexType, double> MutatedAttrMap>
auto attribute_assortativity(
  const NetT& net, const MutatorAttrMap& mutator_attr_map,
  const MutatedAttrMap& mutated_attr_map, double mutator_default_value,
  double mutated_default_value) -> double {
  return attribute_assortativity(
    net,
    [&mutator_attr_map, mutator_default_value](VertexType v) -> double {
      auto it = mutator_attr_map.find(v);
      if (it == mutator_attr_map.end())
        return mutator_default_value;
      else
        return it->second;
    },
    [&mutated_attr_map, mutated_default_value](VertexType v) -> double {
      auto it = mutated_attr_map.find(v);
      if (it == mutated_attr_map.end())
        return mutated_default_value;
      else
        return it->second;
    });
}

template <static_network_like NetT>
  requires is_undirected_v<NetT>
auto degree_assortativity(const NetT& net) -> double {
  return attribute_assortativity(
    net, [&net](VertexType v) { return static_cast<double>(net.degree(v)); });
}

template <static_network_like NetT>
auto degree_assortativity(const NetT& net, direction tail, direction head)
  -> double {
  std::function<double(VertexType)> in_d = [&net](VertexType v) {
    return static_cast<double>(net.in_degree(v));
  };

  std::function<double(VertexType)> out_d = [&net](VertexType v) {
    return static_cast<double>(net.out_degree(v));
  };

  return attribute_assortativity(
    net, tail == direction::in ? in_d : out_d,
    head == direction::in ? in_d : out_d);
}
} // namespace reticula
