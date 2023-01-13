#include <functional>

#include "../include/reticula/utils.hpp"

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
  template <std::ranges::input_range R1, std::ranges::input_range R2>
  requires
    std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
  directed_hyperedge<VertexType>::directed_hyperedge(
      const R1& tails, const R2& heads) {
    if constexpr (std::ranges::sized_range<R1>)
      _heads.reserve(std::ranges::size(heads));
    std::ranges::copy(heads, std::back_inserter(_heads));

    if constexpr (std::ranges::sized_range<R2>)
      _tails.reserve(std::ranges::size(tails));
    std::ranges::copy(tails, std::back_inserter(_tails));

    std::ranges::sort(_heads);
    auto [hfirst, hlast] = std::ranges::unique(_heads);
    _heads.erase(hfirst, hlast);
    _heads.shrink_to_fit();

    std::ranges::sort(_tails);
    auto [tfirst, tlast] = std::ranges::unique(_tails);
    _tails.erase(tfirst, tlast);
    _tails.shrink_to_fit();
  }

  template <network_vertex VertexType>
  inline bool directed_hyperedge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return std::ranges::binary_search(_tails, vert);
  }

  template <network_vertex VertexType>
  inline bool directed_hyperedge<VertexType>::is_in_incident(
      const VertexType& vert) const  {
    return std::ranges::binary_search(_heads, vert);
  }

  template <network_vertex VertexType>
  inline bool directed_hyperedge<VertexType>::is_incident(
      const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_hyperedge<VertexType>::mutator_verts() const {
    return _tails;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_hyperedge<VertexType>::mutated_verts() const {
    return _heads;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_hyperedge<VertexType>::tails() const {
    return _tails;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_hyperedge<VertexType>::heads() const {
    return _heads;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_hyperedge<VertexType>::incident_verts() const {
    std::vector<VertexType> res;
    res.reserve(_heads.size() + _tails.size());
    std::ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

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
    std::ranges::set_intersection(a._heads, b._tails,
        std::back_inserter(common));
    return common.size() > 0;
  }

  // properties of undirected hyperedge:

  template <network_vertex VertexType>
  undirected_hyperedge<VertexType>::undirected_hyperedge(
      std::initializer_list<VertexType> verts) :
    undirected_hyperedge(std::vector<VertexType>(verts)) {}

  template <network_vertex VertexType>
  template <std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, VertexType>
  undirected_hyperedge<VertexType>::undirected_hyperedge(
      const R& verts) : _verts(verts) {
    if constexpr (std::ranges::sized_range<R>)
      _verts.reserve(std::ranges::size(verts));
    std::ranges::copy(verts, std::back_inserter(_verts));

    std::ranges::sort(_verts);
    auto [first, last] = std::ranges::unique(_verts);
    _verts.erase(first, last);
    _verts.shrink_to_fit();
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_incident(
      const VertexType& vert) const {
    return std::ranges::binary_search(_verts, vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_in_incident(
      const VertexType& vert) const {
    return std::ranges::binary_search(_verts, vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return std::ranges::binary_search(_verts, vert);
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_hyperedge<VertexType>::mutator_verts() const {
    return _verts;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_hyperedge<VertexType>::mutated_verts() const {
    return _verts;
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_hyperedge<VertexType>::incident_verts() const {
    return _verts;
  }

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
    std::ranges::set_intersection(a._verts, b._verts,
        std::back_inserter(common));
    return common.size() > 0;
  }
}  // namespace reticula
