#ifndef INCLUDE_RETICULA_STATIC_EDGES_HPP_
#define INCLUDE_RETICULA_STATIC_EDGES_HPP_

#include <tuple>
#include <vector>
#include <algorithm>

#include <hll/hyperloglog.hpp>

#include "type_traits.hpp"
#include "network_concepts.hpp"

namespace reticula {
  template <network_vertex VertT>
  class directed_edge;

  template<network_vertex VertT>
  struct is_instantaneous<directed_edge<VertT>> {
    static constexpr bool value = true;
  };

  template<network_vertex VertT>
  struct is_undirected<directed_edge<VertT>> {
    static constexpr bool value = false;
  };

  template<network_vertex VertT>
  struct is_dyadic<directed_edge<VertT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT>
  class undirected_edge;

  template<network_vertex VertT>
  struct is_instantaneous<undirected_edge<VertT>> {
    static constexpr bool value = true;
  };

  template<network_vertex VertT>
  struct is_undirected<undirected_edge<VertT>> {
    static constexpr bool value = true;
  };

  template<network_vertex VertT>
  struct is_dyadic<undirected_edge<VertT>> {
    static constexpr bool value = true;
  };

  /**
    A directed edges (or link) indicate unsymertic relation or orientation in
    the relationship between the two nodes. Tail and head vertices allude to a
    representation of the edge as an arrow from one vertex to another.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
  */
  template <network_vertex VertT>
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
    directed_edge(VertexType v1, VertexType v2);


    /**
      A directed edge is out_incident to vertex `v` iff `v` is the tail vertex
      of the edge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    inline bool
    is_out_incident(const VertexType& vert) const;

    /**
      A directed edge is in_incident to vertex `v` iff `v` is the head vertex
      of the edge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    inline bool
    is_in_incident(const VertexType& vert) const;

    /**
      A directed edge is incident to vertex `v` iff `v` is the head or tail
      vertex of the edge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_incident(const VertexType& vert) const;

    /**
      List of all vertices that can initiate (cause) the effects of the
      relationship. For directed edges this is equal to the tail vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed edges this is equal to the head vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      List of all vertices that initiate (cause) or receive (affected by) the
      effects of the relationship. For directed edges this is equal to the union
      of results of `mutator_verts()` and `mutated_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      Returns the single vertex at the tail of the directed link arrow.
     */
    [[nodiscard]]
    VertexType tail() const;

    /**
      Returns the single vertex at the head of the directed link arrow.
     */
    [[nodiscard]]
    VertexType head() const;

    /**
      Defines a strong lexicographic ordering where tail vertices compared
      before head vertices.
     */
    auto operator<=>(const directed_edge<VertexType>& other) const = default;

    /**
      Defines a strong lexicographic ordering along with `operator==` where head
      vertices compared before tail vertices.
     */
    template <network_vertex VertexType>
    friend bool effect_lt(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b);

    /**
      Two directed edges are adjacent if head of the first one is the tail of
      the second one. Lack of an adjacency relation between edges ususlly mean
      that an effect transmitted through one edge logically cannot be
      transmitted through the other.
     */
    template <network_vertex VertexType>
    friend bool adjacent(
        const directed_edge<VertexType>& a,
        const directed_edge<VertexType>& b);

    /**
      Inserts undirected edge formatted as `v1 v2` where `v1` and `v2` are
      tail and head vertex respectively.
     */
    template <network_vertex VertexType>
    friend std::ostream& operator<<(
        std::ostream &os,
        const directed_edge<VertexType>& e);

    /**
      Extracts directed edge from an input stream formatted as `v1 v2` where
      `v1` and `v2` are tail and head vertex respectively.
     */
    template <network_vertex VertexType>
    friend std::istream& operator>>(
        std::istream &is,
        directed_edge<VertexType>& e);

  private:
    VertexType _tail, _head;

    friend struct std::hash<directed_edge<VertexType>>;
    friend struct hll::hash<directed_edge<VertexType>>;
  };


  /**
    An undirected edges (or link) indicate symertic relation or lack of
    orientation in the relationship between the two nodes.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
  */
  template <network_vertex VertT>
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
    undirected_edge(VertexType v1, VertexType v2);

    /**
      An undirected edge is incident to vertex `v` iff `v` is either of its
      vertices.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_incident(const VertexType& vert) const;


    /**
      Exactly the same as `is_incident`.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_in_incident(const VertexType& vert) const;

    /**
      Exactly the same as `is_incident`.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_out_incident(const VertexType& vert) const;

    /**
      In an undirected edge both vertices might act as source or cause of an
      effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      In an undirected edge both vertices might act as target of an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      In an undirected edge both vertices might act are considered incident.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      Defines a arbitrary strong ordering.
     */
    auto operator<=>(const undirected_edge<VertexType>&) const = default;

    /**
      Exactly the same as `operator<`.
     */
    template <network_vertex VertexType>
    friend bool effect_lt(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b);

    /**
      Two undirected edges are adjacent if they have at least one node in
      common. Lack of an adjacency relation between edges ususlly mean that an
      effect transmitted through one edge logically cannot be transmitted
      through the other.
     */
    template <network_vertex VertexType>
    friend bool adjacent(
        const undirected_edge<VertexType>& a,
        const undirected_edge<VertexType>& b);

    /**
      Inserts undirected edge formatted as `v1 v2` where `v1` and `v2` are
      vertices in the same order as initialiser.
     */
    template <network_vertex VertexType> requires output_streamable<VertexType>
    friend std::ostream& operator<<(std::ostream &os,
        const undirected_edge<VertexType>& e);

    /**
      Extracts undirected edge from an input stream formatted as `v1 v2`.
     */
    template <network_vertex VertexType> requires input_streamable<VertexType>
    friend std::istream& operator>>(std::istream &is,
        undirected_edge<VertexType>& e);

  private:
    VertexType _v1, _v2;

    friend struct std::hash<undirected_edge<VertexType>>;
    friend struct hll::hash<undirected_edge<VertexType>>;
  };
}  // namespace reticula

#include "../../src/static_edges.tpp"

#endif  // INCLUDE_RETICULA_STATIC_EDGES_HPP_
