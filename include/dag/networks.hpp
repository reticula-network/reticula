#ifndef DAG_NETWORKS_H
#define DAG_NETWORKS_H

#include <unordered_set>

#include "static_edges.hpp"
#include "temporal_edges.hpp"

namespace dag {
  template <typename EdgeT>
  class network {
    std::unordered_set<EdgeT> edge_list;
  public:
    using VertexType = typename EdgeT::VertexType;
    using EdgeType = EdgeT;
    network() = default;
    std::unordered_set<VertexType> vertices() const;

    inline void add_edge(const EdgeT& new_edge) {
      edge_list.insert(new_edge);
    };

    void add_edges(const std::vector<EdgeT>& new_edges);

    inline const std::unordered_set<EdgeT>& edges() const { return edge_list; }
    inline void reserve(size_t count) { edge_list.reserve(count); }

    std::unordered_map<VertexType, std::unordered_set<EdgeT>>
      in_edges() const;
    std::unordered_map<VertexType, std::unordered_set<EdgeT>>
      out_edges() const;
    std::unordered_map<VertexType, std::unordered_set<EdgeT>>
      incident_edges() const;

    std::unordered_map<VertexType, size_t> in_degree() const;
    std::unordered_map<VertexType, size_t> out_degree() const;
    std::unordered_map<VertexType, size_t> degree() const;

    std::unordered_map<VertexType, std::unordered_set<VertexType>>
      predecessors() const;
    std::unordered_map<VertexType, std::unordered_set<VertexType>>
      successors() const;
    std::unordered_map<VertexType, std::unordered_set<VertexType>>
      neighbours() const;

    std::unordered_set<EdgeT> in_edges(const VertexType vert) const;
    std::unordered_set<EdgeT> out_edges(const VertexType vert) const;
    std::unordered_set<EdgeT> incident_edges(const VertexType vert) const;
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
}

#include "../../src/networks.tpp"

#endif /* DAG_NETWORKS_H */
