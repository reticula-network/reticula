#ifndef INCLUDE_RETICULA_STATIC_HYPEREDGES_HPP_
#define INCLUDE_RETICULA_STATIC_HYPEREDGES_HPP_

#include <tuple>
#include <vector>
#include <algorithm>
#include <ranges>
#include <initializer_list>

#include <hll/hyperloglog.hpp>

#include "type_traits.hpp"
#include "network_concepts.hpp"

namespace reticula {
  template <network_vertex VertT>
  class directed_hyperedge;

  template<network_vertex VertT>
  struct is_instantaneous<directed_hyperedge<VertT>> {
    static constexpr bool value = true;
  };

  template<network_vertex VertT>
  struct is_undirected<directed_hyperedge<VertT>> {
    static constexpr bool value = false;
  };

  template<network_vertex VertT>
  struct is_dyadic<directed_hyperedge<VertT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT>
  class undirected_hyperedge;

  template<network_vertex VertT>
  struct is_instantaneous<undirected_hyperedge<VertT>> {
    static constexpr bool value = true;
  };

  template<network_vertex VertT>
  struct is_undirected<undirected_hyperedge<VertT>> {
    static constexpr bool value = true;
  };

  template<network_vertex VertT>
  struct is_dyadic<undirected_hyperedge<VertT>> {
    static constexpr bool value = false;
  };


  /**
    A directed hyperedges (or hyperlink) indicate unsymertic relation or
    orientation in the relationship between the two set of nodes. Tail and head
    set of vertices allude to a representation of the hyperedge as an arrow from
    one set of vertices to another.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
  */
  template <network_vertex VertT>
  class directed_hyperedge {
  public:
    /**
      Type used for labelling vertices.
     */
    using VertexType = VertT;

    directed_hyperedge() = default;

    /**
      Create an directed hyperedge. This variation is specifically created so
      that a brace-enclosed initializer list can be used to initialize this
      class.

      @param tails The set of vertices at the tail end of the hyperedge, often
      the initiator or cause of the action or the relation.
      @param heads The set of vertices at the head end of the hyperedge, often
      the receiving end of an effect.
     */
    directed_hyperedge(
        std::initializer_list<VertexType> tails,
        std::initializer_list<VertexType> heads);

    /**
      Create a directed hyperedge.

      @param tails The set of vertices at the tail end of the hyperedge, often
      the initiator or cause of the action or the relation.
      @param heads The set of vertices at the head end of the hyperedge, often
      the receiving end of an effect.
     */
    template <std::ranges::input_range R1, std::ranges::input_range R2>
    requires
      std::convertible_to<std::ranges::range_value_t<R1>, VertT> &&
      std::convertible_to<std::ranges::range_value_t<R2>, VertT>
    directed_hyperedge(const R1& tails, const R2& heads);


    /**
      A directed hyperedge is out_incident to vertex `v` iff `v` is in the tail
      set of vertices of the hyperedge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    inline bool
    is_out_incident(const VertexType& vert) const;

    /**
      A directed hyperedge is in_incident to vertex `v` iff `v` is in the head
      set of vertices of the hyperedge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    inline bool
    is_in_incident(const VertexType& vert) const;

    /**
      A directed hyperedge is incident to vertex `v` iff `v` is in the head or
      tail set of vertices of the hyperedge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    inline bool is_incident(const VertexType& vert) const;

    /**
      List of all vertices that can initiate (cause) the effects of the
      relationship. For directed hyperedges this is equal to the tail set of
      vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed hyperedges this is equal to the head set of
      vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      List of all vertices that can initiate (cause) or receive (be affected by)
      the effects of the relationship. For directed hyperedges this is equal to
      the union of tail and head sets of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      The same as `mutated_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> tails() const;

    /**
      The same as `mutator_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> heads() const;

    /**
      Defines a strong lexicographic ordering where tail vertices compared
      before head vertices.
     */
    auto operator<=>(const directed_hyperedge<VertexType>&) const = default;

    /**
      Defines a strong lexicographic ordering along with `operator==` where
      head vertices set are compared before tail vertices set.
     */
    template <network_vertex VertexType>
    friend bool effect_lt(
        const directed_hyperedge<VertexType>& a,
        const directed_hyperedge<VertexType>& b);

    /**
      Two directed hyperedges are adjacent if a head vertex of the first one is
      a tail vertex of the second one. Lack of an adjacency relation between
      hyperedges ususlly mean that an effect transmitted through one hyperedge
      logically cannot be transmitted through the other.
     */
    template <network_vertex VertexType>
    friend bool adjacent(
        const directed_hyperedge<VertexType>& a,
        const directed_hyperedge<VertexType>& b);

  private:
    std::vector<VertexType> _tails, _heads;

    friend struct std::hash<directed_hyperedge<VertexType>>;
    friend struct hll::hash<directed_hyperedge<VertexType>>;
  };

  /**
    An undirected hyperedges (or hyperlink) indicate symertic relation or
    lack of orientation in the relationship between the two or more nodes.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
  */
  template <network_vertex VertT>
  class undirected_hyperedge {
  public:
    /**
      Type used for labelling vertices
     */
    using VertexType = VertT;

    undirected_hyperedge() = default;

    /**
      Create an undirected hyperedge. Order of the vertices is arbitrary. This
      variation is specifically created so that a brace-enclosed initializer
      list can be used to initialize this class.
     */
    undirected_hyperedge(std::initializer_list<VertexType> verts);

    /**
      Create an undirected hyperedge. Order of the vertices is arbitrary.
     */
    template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, VertT>
    explicit undirected_hyperedge(const R& verts);

    /**
      An undirected hyperedge is incident to vertex `v` iff `v` is one of its
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
      In an undirected hyoeredge all vertices might act as source or cause of an
      effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      In an undirected hyperedge all vertices might act as target of an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      In an undirected hyperedge all vertices are incident to the edge.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      Defines an arbitrary strong ordering.
     */
    auto operator<=>(const undirected_hyperedge<VertexType>&) const = default;

    /**
      Exactly the same as `operator<`.
     */
    template <network_vertex VertexType>
    friend bool effect_lt(
        const undirected_hyperedge<VertexType>& a,
        const undirected_hyperedge<VertexType>& b);

    /**
      Two undirected hyperedges are adjacent if they have at least one node in
      common. Lack of an adjacency relation between hyperedges ususlly mean that
      an effect transmitted through one hyperedge logically cannot be
      transmitted through the other.
     */
    template <network_vertex VertexType>
    friend bool adjacent(
        const undirected_hyperedge<VertexType>& a,
        const undirected_hyperedge<VertexType>& b);

  private:
    std::vector<VertexType> _verts;

    friend struct std::hash<undirected_hyperedge<VertexType>>;
    friend struct hll::hash<undirected_hyperedge<VertexType>>;
  };
}  // namespace reticula

#include "../../src/static_hyperedges.tpp"
#endif  // INCLUDE_RETICULA_STATIC_HYPEREDGES_HPP_
