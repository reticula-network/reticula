#ifndef INCLUDE_RETICULA_STATIC_EDGES_HPP_
#define INCLUDE_RETICULA_STATIC_EDGES_HPP_

#include <tuple>
#include <span>
#include <array>
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
      Create a directed edge from mutator and mutated vertex ranges.

      @param tag Uniform construction tag.
      @param mutators Range of mutator vertices (tail vertices).
      @param mutated Range of mutated vertices (head vertices).
     */
    template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
    requires
      std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
      std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
    directed_edge(
        uniform_const_t,
        MutatorRange&& mutators,
        MutatedRange&& mutated);


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
    std::span<const VertexType, 1> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed edges this is equal to the head vertex.
     */
    [[nodiscard]]
    std::span<const VertexType, 1> mutated_verts() const;

    /**
      List of all vertices that initiate (cause) or receive (affected by) the
      effects of the relationship. For directed edges this is equal to the union
      of results of `mutator_verts()` and `mutated_verts()`.
     */
    [[nodiscard]]
    std::span<const VertexType> incident_verts() const;

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
    // _tail, _head;
    std::array<VertexType, 2> _verts;

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
      Create an undirected edge from mutator and mutated vertex ranges.

      @param tag Uniform construction tag.
      @param mutators Range of mutator vertices.
      @param mutated Range of mutated vertices.
     */
    template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
    requires
      std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
      std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
    undirected_edge(
        uniform_const_t,
        MutatorRange&& mutators,
        MutatedRange&& mutated);

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
    std::span<const VertexType> mutator_verts() const;

    /**
      In an undirected edge both vertices might act as target of an effect.
     */
    [[nodiscard]]
    std::span<const VertexType> mutated_verts() const;

    /**
      In an undirected edge both vertices might act are considered incident.
     */
    [[nodiscard]]
    std::span<const VertexType> incident_verts() const;

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
    std::array<VertexType, 2> _verts;

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
          reticula::hash<VertexType>{}(e._verts[0]), e._verts[1]);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_edge<VertexType>> {
    std::size_t operator()(
        const reticula::undirected_edge<VertexType>& e) const {
      return reticula::utils::combine_hash<VertexType, reticula::hash>(
          reticula::hash<VertexType>{}(e._verts[0]), e._verts[1]);
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
      : _verts({tail, head}) {}

  template <network_vertex VertexType>
  template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
  requires
    std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
    std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
  directed_edge<VertexType>::directed_edge(
      uniform_const_t,
      MutatorRange&& mutators,
      MutatedRange&& mutated) {
    std::vector<VertexType> mutator_verts(ranges::begin(mutators), ranges::end(mutators));
    std::vector<VertexType> mutated_verts(ranges::begin(mutated), ranges::end(mutated));

    if (mutator_verts.empty() || mutated_verts.empty())
      throw std::invalid_argument(
        "directed_edge requires non-empty mutator and mutated ranges");

    if (mutator_verts.size() != 1 || mutated_verts.size() != 1)
      throw std::invalid_argument(
        "directed_edge requires exactly one element in each range");

    _verts[0] = mutator_verts[0];
    _verts[1] = mutated_verts[0];
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_out_incident(const VertexType& vert) const {
    return (_verts[0] == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_in_incident(const VertexType& vert) const  {
    return (_verts[1] == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_incident(const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType>
  std::span<const VertexType, 1>
  directed_edge<VertexType>::mutator_verts() const {
    return std::span<const VertexType, 1>(&_verts[0], 1);
  }

  template <network_vertex VertexType>
  std::span<const VertexType, 1>
  directed_edge<VertexType>::mutated_verts() const {
    return std::span<const VertexType, 1>(&_verts[1], 1);
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  directed_edge<VertexType>::incident_verts() const {
    std::size_t n = (_verts[0] == _verts[1]) ? 1 : 2;
    return {_verts.data(), n};
  }

  template <network_vertex VertexType>
  VertexType directed_edge<VertexType>::tail() const {
    return _verts[0];
  }

  template <network_vertex VertexType>
  VertexType directed_edge<VertexType>::head() const {
    return _verts[1];
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType>
  auto directed_edge<VertexType>::operator<=>(
      const directed_edge<VertexType>& o) const {
    return utils::compare(
      std::tie(_verts[0], _verts[1]), std::tie(o._verts[0], o._verts[1]));
  }
#endif

  template <network_vertex VertexType>
  bool effect_lt(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return std::make_tuple(a._verts[1], a._verts[0]) <
      std::make_tuple(b._verts[1], b._verts[0]);
  }

  template <network_vertex VertexType>
  bool adjacent(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a._verts[1] == b._verts[0]);
  }

  template <network_vertex VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const directed_edge<VertexType>& e) {
    return os << e._verts[0] << " " << e._verts[1];
  }

  template <network_vertex VertexType>
  std::istream& operator>>(
      std::istream &is,
      directed_edge<VertexType>& e) {
    return is >> e._verts[0] >> e._verts[1];
  }


  // properties of undirected edge:

  template <network_vertex VertexType>
  undirected_edge<VertexType>::undirected_edge(
      const VertexType& v1, const VertexType& v2) {
    auto [l, h] = std::minmax(v1, v2);
    _verts = {l, h};
  }

  template <network_vertex VertexType>
  template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
    requires std::convertible_to<
               ranges::range_value_t<MutatorRange>, VertexType> &&
             std::convertible_to<
               ranges::range_value_t<MutatedRange>, VertexType>
  undirected_edge<VertexType>::undirected_edge(
    uniform_const_t, MutatorRange&& mutators, MutatedRange&& mutated) {
    std::vector<VertexType> mutator_verts(
      ranges::begin(mutators), ranges::end(mutators));
    std::vector<VertexType> mutated_verts(
      ranges::begin(mutated), ranges::end(mutated));

    if (mutator_verts.empty() || mutated_verts.empty())
      throw std::invalid_argument(
        "undirected_edge requires non-empty mutator and mutated ranges");

    if (!ranges::equal(mutator_verts, mutated_verts))
      throw std::invalid_argument(
        "undirected_edge requires mutator and mutated ranges to be equal");

    if (mutator_verts.size() == 1) {
      _verts = {mutator_verts[0], mutator_verts[0]};
    } else if (mutator_verts.size() == 2) {
      auto [l, h] = std::minmax(mutator_verts[0], mutator_verts[1]);
      _verts = {l, h};
    } else {
      throw std::invalid_argument(
        "undirected_edge requires exactly 1 or 2 vertices");
    }
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert || _verts[1] == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_in_incident(
      const VertexType& vert) const {
    return is_incident(vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return is_incident(vert);
  }

  template <network_vertex vertextype>
  std::span<const vertextype>
  undirected_edge<vertextype>::incident_verts() const {
    std::size_t n = (_verts[0] == _verts[1]) ? 1 : 2;
    return {_verts.data(), n};
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  undirected_edge<VertexType>::mutator_verts() const {
    return incident_verts();
  }

  template <network_vertex vertextype>
  std::span<const vertextype>
  undirected_edge<vertextype>::mutated_verts() const {
    return incident_verts();
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType>
  auto undirected_edge<VertexType>::operator<=>(
      const undirected_edge<VertexType>& o) const {
    return utils::compare(
      std::tie(_verts[0], _verts[1]), std::tie(o._verts[0], o._verts[1]));
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
    return (a._verts[0] == b._verts[0] ||
            a._verts[0] == b._verts[1] ||
            a._verts[1] == b._verts[0] ||
            a._verts[1] == b._verts[1]);
  }

  template <network_vertex VertexType>
  requires output_streamable<VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const undirected_edge<VertexType>& e) {
    return os << e._verts[0] << " " << e._verts[1];
  }

  template <network_vertex VertexType>
  requires input_streamable<VertexType>
  std::istream& operator>>(std::istream &is,
      undirected_edge<VertexType>& e) {
    return is >> e._verts[0] >> e._verts[1];
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_STATIC_EDGES_HPP_
