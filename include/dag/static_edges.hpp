#ifndef INCLUDE_DAG_STATIC_EDGES_HPP_
#define INCLUDE_DAG_STATIC_EDGES_HPP_

#include <tuple>
#include <vector>
#include <algorithm>

namespace dag {
  template <typename VertT>
  class directed_edge;

  template <typename VertT>
  class undirected_edge;

  template <class VertT>
  bool effect_lt(
      const directed_edge<VertT>& a,
      const directed_edge<VertT>& b) {
      return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <class VertT>
  bool effect_lt(
      const undirected_edge<VertT>& a,
      const undirected_edge<VertT>& b) {
      return (a < b);
  }

  /**
    A directed edges (or link) indicate unsymertic relation or orientation in
    the relationship between the two nodes. Tail and head vertices allude to a
    representation of the edge as an arrow from one vertex to another.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
  */
  template <typename VertT>
  class directed_edge {
  public:
    /**
      Type used for labelling vertices.
     */
    using VertexType = VertT;

    directed_edge() = default;
    /**
      Create a directed edge.

      @param v1 Tail end of the edge, often the initiator or cause of the action
      or the relation.
      @param v2 Head end of the edge, often the receiving end of an effect.
     */
    directed_edge(const VertexType v1, const VertexType v2)
      : v1(v1), v2(v2) {}


    /**
      A directed edge is out_incident to vertex `v` iff `v` is the tail vertex
      of the edge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    inline bool
    is_out_incident(const VertexType vert) const { return (v1 == vert); }

    /**
      A directed edge is in_incident to vertex `v` iff `v` is the head vertex
      of the edge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    inline bool
    is_in_incident(const VertexType vert) const  { return (v2 == vert); }

    /**
      A directed edge is incident to vertex `v` iff `v` is the head or tail
      vertex of the edge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_incident(const VertexType vert) const {
      return (is_out_incident(vert) || is_in_incident(vert));
    }

    /**
      List of all vertices that initiate (cause) the effects of the
      relationship. For directed edges this is equal to the tail vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const { return {v1}; }

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed edges this is equal to the head vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const { return {v2}; }

    /**
      Simply defined as negation of equal operator `operator==`.
     */
    [[nodiscard]]
    friend bool operator!=(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b) { return !(a == b); }

    /**
      Two directed edges are equal if their head and tail vertices are
      correspondingly equal to each others.
     */
    [[nodiscard]]
    friend bool operator==(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b) {
      return (a.cause_comp_tuple() == b.cause_comp_tuple());
    }

    /**
      Defines a strong lexicographic ordering along with `operator==` where tail
      vertices compared before head vertices.
     */
    [[nodiscard]]
    friend bool operator<(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b) {
      return (a.cause_comp_tuple() < b.cause_comp_tuple());
    }

    /**
      Defines a strong lexicographic ordering along with `operator==` where head
      vertices compared before tail vertices.
     */
    [[nodiscard]]
    friend bool ::dag::effect_lt<>(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b);

    /**
      Two directed edges are adjacent if head of the first one is the tail of
      the second one. Lack of an adjacency relation between edges ususlly mean
      that an effect transmitted through one edge logically cannot be
      transmitted through the other.
     */
    [[nodiscard]]
    friend bool adjacent(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b) {
      return (a.v2 == b.v1);
    }

    /**
      Inserts undirected edge formatted as `v1 v2` where `v1` and `v2` are
      tail and head vertex respectively.
     */
    friend std::ostream& operator<<(std::ostream &os,
        const directed_edge<VertexType>& e) {
      return os << e.v1 << " " << e.v2;
    }

    /**
      Extracts directed edge from an input stream formatted as `v1 v2` where
      `v1` and `v2` are tail and head vertex respectively.
     */
    friend std::istream& operator>>(std::istream &is,
        directed_edge<VertexType>& e) {
      return is >> e.v1 >> e.v2;
    }

  private:
    VertexType v1, v2;
    [[nodiscard]]
    inline std::tuple<VertexType, VertexType> cause_comp_tuple() const {
      return std::make_tuple(v1, v2);
    }

    [[nodiscard]]
    inline std::tuple<VertexType, VertexType> effect_comp_tuple() const {
      return std::make_tuple(v2, v1);
    }

    friend struct std::hash<directed_edge<VertexType>>;
  };


  /**
    An undirected edges (or link) indicate symertic relation or lack of
    orientation in the relationship between the two nodes.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
  */
  template <typename VertT>
  class undirected_edge {
  public:
    /**
      Type used for labelling vertices
     */
    using VertexType = VertT;

    undirected_edge() = default;
    /**
      Create an undirected edge. Order of the parameters are arbitrary unless
      dealing with IO functions. IO functions try to preserve the same order as
      of vertices from input string in the output string.
     */
    undirected_edge(const VertexType v1, const VertexType v2)
      : v1(v1), v2(v2) {}

    /**
      An undirected edge is incident to vertex `v` iff `v` is either of its
      vertices.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_incident(const VertexType vert) const {
      return (v1 == vert || v2 == vert);
    }

    /**
      In an undirected edge both edges might act as source or cause of an
      effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const { return {v1, v2}; }

    /**
      In an undirected edge both edges might act as target of an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const { return {v1, v2}; }

    /**
      Simply defined as negation of equal operator `operator==`.
     */
    [[nodiscard]]
    friend bool operator!=(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b) { return !(a == b); }

    /**
      Two directed edges are equal if their (unordered) set of vertices are
      equal.
     */
    [[nodiscard]]
    friend bool operator==(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b) {
      return (a.comp_tuple() == b.comp_tuple());
    }

    /**
      Defines an arbitrary weak ordering along with `operator==`.
     */
    [[nodiscard]]
    friend bool operator<(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b) {
      return (a.comp_tuple() < b.comp_tuple());
    }

    /**
      Exactly the same as `operator<`.
     */
    [[nodiscard]]
    friend bool ::dag::effect_lt<>(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b);

    /**
      Two undirected edges are adjacent if they have at least one node in
      common. Lack of an adjacency relation between edges ususlly mean that an
      effect transmitted through one edge logically cannot be transmitted
      through the other.
     */
    [[nodiscard]]
    friend bool adjacent(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b) {
      return (a.v1 == b.v1 ||
              a.v1 == b.v2 ||
              a.v2 == b.v1 ||
              a.v2 == b.v2);
    }

    /**
      Inserts undirected edge formatted as `v1 v2` where `v1` and `v2` are
      vertices in the same order as initialiser.
     */
    friend std::ostream& operator<<(std::ostream &os,
        const undirected_edge<VertexType>& e) {
      return os << e.v1 << " " << e.v2;
    }

    /**
      Extracts undirected edge from an input stream formatted as `v1 v2`.
     */
    friend std::istream& operator>>(std::istream &is,
        undirected_edge<VertexType>& e) {
      return is >> e.v1 >> e.v2;
    }

  private:
    VertexType v1, v2;
    [[nodiscard]]
    inline std::tuple<VertexType, VertexType> comp_tuple() const {
      return std::make_tuple(std::min(v1, v2), std::max(v1, v2));
    }

    friend struct std::hash<undirected_edge<VertexType>>;
  };
}  // namespace dag

#include "../../src/static_edges.tpp"

#endif  // INCLUDE_DAG_STATIC_EDGES_HPP_
