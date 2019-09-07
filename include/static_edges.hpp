#ifndef DAG_STATIC_EDGES_H
#define DAG_STATIC_EDGES_H

namespace dag {
  template <typename VertT>
  class directed_edge {
  public:
    using VertexType = VertT;
    VertT v1, v2;

    directed_edge() = default;
    directed_edge(const VertT v1, const VertT v2)
      : v1(v1), v2(v2) {}

    [[nodiscard]]
    inline bool operator==(const directed_edge<VertT>& other) const {
      auto t = std::tie(v1, v2);
      auto o = std::tie(other.v1, other.v2);
      return (t == o);
    }

    [[nodiscard]]
    inline bool operator!=(const directed_edge<VertT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline VertT head_vert() const { return v2; }

    [[nodiscard]]
    inline VertT tail_vert() const { return v1; }

    [[nodiscard]]
    inline bool is_out_incident(const VertT vert) const { return (v1 == vert); }

    [[nodiscard]]
    inline bool is_in_incident(const VertT vert) const  { return (v2 == vert); }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (is_out_incident(vert) || is_in_incident(vert));
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v2}; }
  };


  template <typename VertT>
  class undirected_edge {
  public:
    using VertexType = VertT;
    VertT v1, v2;

    undirected_edge() = default;
    undirected_edge(const VertT v1, const VertT v2)
      : v1(v1), v2(v2) {}

    [[nodiscard]]
    inline bool operator==(const undirected_edge<VertT>& other) const {
      auto t = std::tie(v1, v2);
      auto o1 = std::tie(other.v1, other.v2);
      auto o2 = std::tie(other.v2, other.v1);
      return (t == o1 || t == o2);
    }

    [[nodiscard]]
    inline bool operator!=(const undirected_edge<VertT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1, v2}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v1, v2}; }
  };
}

#include "../src/static_edges.tpp"

#endif /* DAG_STATIC_EDGES_H */
