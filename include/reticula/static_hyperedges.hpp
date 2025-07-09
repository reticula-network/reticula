#ifndef INCLUDE_RETICULA_STATIC_HYPEREDGES_HPP_
#define INCLUDE_RETICULA_STATIC_HYPEREDGES_HPP_

#include <tuple>
#include <vector>
#include <span>
#include <algorithm>
#include <compare>
#include <initializer_list>

#include <hll/hyperloglog.hpp>

#include "ranges.hpp"
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
    template <ranges::input_range R1, ranges::input_range R2>
    requires
      std::convertible_to<ranges::range_value_t<R1>, VertT> &&
      std::convertible_to<ranges::range_value_t<R2>, VertT>
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
    std::span<const VertexType> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed hyperedges this is equal to the head set of
      vertices.
     */
    [[nodiscard]]
    std::span<const VertexType> mutated_verts() const;

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
    std::span<const VertexType> tails() const;

    /**
      The same as `mutator_verts()`.
     */
    [[nodiscard]]
    std::span<const VertexType> heads() const;

    /**
      Defines a strong lexicographic ordering where tail vertices compared
      before head vertices.
     */

#if (_LIBCPP_VERSION)
    auto operator<=>(const directed_hyperedge<VertexType>&) const;
    bool operator==(
      const directed_hyperedge<VertexType>&) const noexcept = default;
#else
    auto operator<=>(const directed_hyperedge<VertexType>&) const = default;
#endif

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
    template <ranges::input_range R>
    requires std::convertible_to<ranges::range_value_t<R>, VertT>
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
    std::span<const VertexType> mutator_verts() const;

    /**
      In an undirected hyperedge all vertices might act as target of an effect.
     */
    [[nodiscard]]
    std::span<const VertexType> mutated_verts() const;

    /**
      In an undirected hyperedge all vertices are incident to the edge.
     */
    [[nodiscard]]
    std::span<const VertexType> incident_verts() const;

    /**
      Defines an arbitrary strong ordering.
     */

#if (_LIBCPP_VERSION)
    auto operator<=>(const undirected_hyperedge<VertexType>&) const;
    bool operator==(const undirected_hyperedge<VertexType>&) const
      noexcept = default;
#else
    auto operator<=>(const undirected_hyperedge<VertexType>&) const = default;
#endif

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

// Implementation
#include <functional>
#include <algorithm>

#include "utils.hpp"

// Hashing rules for hyperedges
namespace std {
  template<reticula::network_vertex VertexType>
  struct hash<reticula::directed_hyperedge<VertexType>> {
    std::size_t
    operator()(const reticula::directed_hyperedge<VertexType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), 0ul,
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), 0ul,
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<std::size_t, reticula::hash>(
          heads_hash, tails_hash);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_hyperedge<VertexType>> {
    std::size_t operator()(
        const reticula::undirected_hyperedge<VertexType>& e) const {
      return std::accumulate(
          e._verts.begin(), e._verts.end(), 0ul,
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
    }
  };
}  // namespace std

// HLL hashing rules for hyperedges
namespace hll {
  template<reticula::network_vertex VertexType>
  struct hash<reticula::directed_hyperedge<VertexType>> {
    uint64_t
    operator()(
        const reticula::directed_hyperedge<VertexType>& e,
        std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::directed_hyperedge<VertexType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_hyperedge<VertexType>> {
    uint64_t
    operator()(
        const reticula::undirected_hyperedge<VertexType>& e,
        std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::undirected_hyperedge<VertexType>>{}(e), seed);
    }
  };
}  // namespace hll


namespace reticula {
  // properties of directed hyperedge:

  template <network_vertex VertexType>
  directed_hyperedge<VertexType>::directed_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads) :
    directed_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads)) {}

  template <network_vertex VertexType>
  template <ranges::input_range R1, ranges::input_range R2>
  requires
    std::convertible_to<ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<ranges::range_value_t<R2>, VertexType>
  directed_hyperedge<VertexType>::directed_hyperedge(
      const R1& tails, const R2& heads) {
    if constexpr (ranges::sized_range<R1>)
      _heads.reserve(ranges::size(heads));
    ranges::copy(heads, std::back_inserter(_heads));

    if constexpr (ranges::sized_range<R2>)
      _tails.reserve(ranges::size(tails));
    ranges::copy(tails, std::back_inserter(_tails));

    ranges::sort(_heads);
    auto [hfirst, hlast] = ranges::unique(_heads);
    _heads.erase(hfirst, hlast);
    _heads.shrink_to_fit();

    ranges::sort(_tails);
    auto [tfirst, tlast] = ranges::unique(_tails);
    _tails.erase(tfirst, tlast);
    _tails.shrink_to_fit();
  }

  template <network_vertex VertexType>
  inline bool directed_hyperedge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return std::binary_search(_tails.begin(), _tails.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool directed_hyperedge<VertexType>::is_in_incident(
      const VertexType& vert) const  {
    return std::binary_search(_heads.begin(), _heads.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool directed_hyperedge<VertexType>::is_incident(
      const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  directed_hyperedge<VertexType>::mutator_verts() const {
    return _tails;
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  directed_hyperedge<VertexType>::mutated_verts() const {
    return _heads;
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  directed_hyperedge<VertexType>::tails() const {
    return _tails;
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  directed_hyperedge<VertexType>::heads() const {
    return _heads;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_hyperedge<VertexType>::incident_verts() const {
    std::vector<VertexType> res;
    res.reserve(_heads.size() + _tails.size());
    ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType>
  auto directed_hyperedge<VertexType>::operator<=>(
      const directed_hyperedge<VertexType>& o) const {
    return utils::compare(std::tie(_tails, _heads),
                          std::tie(o._tails, o._heads));
  }
#endif

  template <network_vertex VertexType>
  bool effect_lt(
      const directed_hyperedge<VertexType>& a,
      const directed_hyperedge<VertexType>& b) {
    return std::make_tuple(a._heads, a._tails) <
      std::make_tuple(b._heads, b._tails);
  }

  template <network_vertex VertexType>
  bool adjacent(
      const directed_hyperedge<VertexType>& a,
      const directed_hyperedge<VertexType>& b) {
    std::vector<VertexType> common;
    ranges::set_intersection(a._heads, b._tails,
        std::back_inserter(common));
    return common.size() > 0;
  }

  // properties of undirected hyperedge:

  template <network_vertex VertexType>
  undirected_hyperedge<VertexType>::undirected_hyperedge(
      std::initializer_list<VertexType> verts) :
    undirected_hyperedge(std::vector<VertexType>(verts)) {}

  template <network_vertex VertexType>
  template <ranges::input_range R>
  requires std::convertible_to<ranges::range_value_t<R>, VertexType>
  undirected_hyperedge<VertexType>::undirected_hyperedge(
      const R& verts) {
    if constexpr (ranges::sized_range<R>)
      _verts.reserve(ranges::size(verts));
    ranges::copy(verts, std::back_inserter(_verts));

    ranges::sort(_verts);
    auto [first, last] = ranges::unique(_verts);
    _verts.erase(first, last);
    _verts.shrink_to_fit();
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_incident(
      const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_in_incident(
      const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  undirected_hyperedge<VertexType>::mutator_verts() const {
    return _verts;
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  undirected_hyperedge<VertexType>::mutated_verts() const {
    return _verts;
  }

  template <network_vertex VertexType>
  std::span<const VertexType>
  undirected_hyperedge<VertexType>::incident_verts() const {
    return _verts;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType>
  auto undirected_hyperedge<VertexType>::operator<=>(
      const undirected_hyperedge<VertexType>& o) const {
    return utils::compare(_verts, o._verts);
  }
#endif

  template <network_vertex VertexType>
  bool effect_lt(
      const undirected_hyperedge<VertexType>& a,
      const undirected_hyperedge<VertexType>& b) {
      return (a._verts < b._verts);
  }

  template <network_vertex VertexType>
  bool adjacent(
      const undirected_hyperedge<VertexType>& a,
      const undirected_hyperedge<VertexType>& b) {
    std::vector<VertexType> common;
    ranges::set_intersection(a._verts, b._verts,
        std::back_inserter(common));
    return common.size() > 0;
  }
}  // namespace reticula

#endif  // INCLUDE_RETICULA_STATIC_HYPEREDGES_HPP_
