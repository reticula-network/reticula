#ifndef INCLUDE_RETICULA_STATIC_EDGES_HPP_
#define INCLUDE_RETICULA_STATIC_EDGES_HPP_

#include <tuple>
#include <vector>
#include <algorithm>
#include <compare>

#include <hll/hyperloglog.hpp>

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
    directed_edge(const VertexType& v1, const VertexType& v2);


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
#if (_LIBCPP_VERSION)
    bool operator==(
      const directed_edge<VertexType>&) const noexcept = default;
    auto operator<=>(const directed_edge<VertexType>& o) const;
#else
    auto operator<=>(const directed_edge<VertexType>& other) const = default;
#endif

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
      Create an undirected edge. Order of the parameters are arbitrary.
     */
    undirected_edge(const VertexType& v1, const VertexType& v2);

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
#if (_LIBCPP_VERSION)
    bool operator==(
      const undirected_edge<VertexType>&) const noexcept = default;
    auto operator<=>(const undirected_edge<VertexType>& o) const;
#else
    auto operator<=>(const undirected_edge<VertexType>&) const = default;
#endif

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

// Implementation
#include <functional>
#include <istream>
#include <ostream>

#include "utils.hpp"
#include "network_concepts.hpp"

// Hashing rules for edges
namespace std {
  template<reticula::network_vertex VertexType>
  struct hash<reticula::directed_edge<VertexType>> {
    std::size_t
    operator()(const reticula::directed_edge<VertexType>& e) const {
      return reticula::utils::combine_hash<VertexType, reticula::hash>(
          reticula::hash<VertexType>{}(e._tail), e._head);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_edge<VertexType>> {
    std::size_t operator()(
        const reticula::undirected_edge<VertexType>& e) const {
      return reticula::utils::combine_hash<VertexType, reticula::hash>(
          reticula::hash<VertexType>{}(e._v1), e._v2);
    }
  };
}  // namespace std

// HLL hashing rules for edges
namespace hll {
  template<reticula::network_vertex VertexType>
  struct hash<reticula::directed_edge<VertexType>> {
    uint64_t
    operator()(const reticula::directed_edge<
        VertexType>& e, std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::directed_edge<VertexType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_edge<VertexType>> {
    uint64_t
    operator()(const reticula::undirected_edge<
        VertexType>& e, std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::undirected_edge<VertexType>>{}(e), seed);
    }
  };
}  // namespace hll

namespace reticula {
  // properties of directed edge:

  template <network_vertex VertexType>
  directed_edge<VertexType>::directed_edge(
      const VertexType& tail, const VertexType& head)
      : _tail(tail), _head(head) {}

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_out_incident(const VertexType& vert) const {
    return (_tail == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_in_incident(const VertexType& vert) const  {
    return (_head == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_incident(const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutator_verts() const {
    return {_tail};
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutated_verts() const {
    return {_head};
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::incident_verts() const {
    if (_tail == _head)
      return {_tail};
    else
      return {_tail, _head};
  }

  template <network_vertex VertexType>
  VertexType directed_edge<VertexType>::tail() const {
    return _tail;
  }

  template <network_vertex VertexType>
  VertexType directed_edge<VertexType>::head() const {
    return _head;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType>
  auto directed_edge<VertexType>::operator<=>(
      const directed_edge<VertexType>& o) const {
    return utils::compare(std::tie(_tail, _head), std::tie(o._tail, o._head));
  }
#endif

  template <network_vertex VertexType>
  bool effect_lt(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return std::make_tuple(a._head, a._tail) <
      std::make_tuple(b._head, b._tail);
  }

  template <network_vertex VertexType>
  bool adjacent(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a._head == b._tail);
  }

  template <network_vertex VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const directed_edge<VertexType>& e) {
    return os << e._tail << " " << e._head;
  }

  template <network_vertex VertexType>
  std::istream& operator>>(
      std::istream &is,
      directed_edge<VertexType>& e) {
    return is >> e._tail >> e._head;
  }


  // properties of undirected edge:

  template <network_vertex VertexType>
  undirected_edge<VertexType>::undirected_edge(
      const VertexType& v1, const VertexType& v2) {
    std::tie(_v1, _v2) = std::minmax(v1, v2);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_in_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_edge<VertexType>::mutator_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex vertextype>
  std::vector<vertextype>
  undirected_edge<vertextype>::mutated_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex vertextype>
  std::vector<vertextype>
  undirected_edge<vertextype>::incident_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType>
  auto undirected_edge<VertexType>::operator<=>(
      const undirected_edge<VertexType>& o) const {
    return utils::compare(std::tie(_v1, _v2), std::tie(o._v1, o._v2));
  }
#endif

  template <network_vertex VertexType>
  bool effect_lt(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
      return a < b;
  }

  template <network_vertex VertexType>
  bool adjacent(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return (a._v1 == b._v1 ||
            a._v1 == b._v2 ||
            a._v2 == b._v1 ||
            a._v2 == b._v2);
  }

  template <network_vertex VertexType>
  requires output_streamable<VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const undirected_edge<VertexType>& e) {
    return os << e._v1 << " " << e._v2;
  }

  template <network_vertex VertexType>
  requires input_streamable<VertexType>
  std::istream& operator>>(std::istream &is,
      undirected_edge<VertexType>& e) {
    return is >> e._v1 >> e._v2;
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_STATIC_EDGES_HPP_
