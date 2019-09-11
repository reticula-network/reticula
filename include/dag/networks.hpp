#ifndef INCLUDE_DAG_NETWORKS_HPP_
#define INCLUDE_DAG_NETWORKS_HPP_

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "static_edges.hpp"
#include "temporal_edges.hpp"

namespace dag {
  template <typename EdgeT>
  class network {
  public:
    using VertexType = typename EdgeT::VertexType;
    using EdgeType = EdgeT;

    network() = default;
    explicit network(const std::vector<EdgeT>& edges);

    std::vector<VertexType> vertices() const;
    const std::vector<EdgeType>& edges() const;

    std::vector<EdgeType> in_edges(const VertexType& vert) const;
    std::vector<EdgeType> out_edges(const VertexType& vert) const;
    std::vector<EdgeType> incident_edges(const VertexType& vert) const;

    size_t in_degree(const VertexType& vert) const;
    size_t out_degree(const VertexType& vert) const;
    size_t degree(const VertexType& vert) const;

    std::vector<VertexType> predecessors(const VertexType& v) const;
    std::vector<VertexType> successors(const VertexType& v) const;
    std::vector<VertexType> neighbours(const VertexType& v) const;

  private:
    std::vector<EdgeType> _edges;
    std::unordered_map<VertexType, std::vector<EdgeType>> _in_edges;
    std::unordered_map<VertexType, std::vector<EdgeType>> _out_edges;
  };

  template <typename VertT>
  using directed_network = network<directed_edge<VertT>>;

  template <typename VertT>
  using undirected_network = network<undirected_edge<VertT>>;

  template <typename VertT, typename TimeT>
  using directed_temporal_network =
    network<directed_temporal_edge<VertT, TimeT>>;

  template <typename VertT, typename TimeT>
  using directed_delayed_temporal_network =
    network<directed_delayed_temporal_edge<VertT, TimeT>>;

  template <typename VertT, typename TimeT>
  using undirected_temporal_network =
    network<undirected_temporal_edge<VertT, TimeT>>;
}  // namespace dag

#include "../../src/networks.tpp"

#endif  // INCLUDE_DAG_NETWORKS_HPP_
