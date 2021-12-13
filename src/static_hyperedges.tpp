#include <functional>

#include "../include/dag/utils.hpp"

// Hashing rules for hyperedges
namespace std {
  template<dag::network_vertex VertexType>
  struct hash<dag::directed_hyperedge<VertexType>> {
    std::size_t
    operator()(const dag::directed_hyperedge<VertexType>& e) const {
      std::size_t heads_hash = std::reduce(
          e._heads.begin(), e._heads.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
      std::size_t tails_hash = std::reduce(
          e._tails.begin(), e._tails.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
      return dag::utils::combine_hash<std::size_t, hash>(
          heads_hash, tails_hash);
    }
  };

  template<dag::network_vertex VertexType>
  struct hash<dag::undirected_hyperedge<VertexType>> {
    std::size_t operator()(
        const dag::undirected_hyperedge<VertexType>& e) const {
      return std::reduce(
          e._verts.begin(), e._verts.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
    }
  };
}  // namespace std

// HLL hashing rules for hyperedges
namespace hll {
  template<dag::network_vertex VertexType>
  struct hash<dag::directed_hyperedge<VertexType>> {
    uint64_t
    operator()(
        const dag::directed_hyperedge<VertexType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::directed_hyperedge<VertexType>>{}(e), seed);
    }
  };

  template<dag::network_vertex VertexType>
  struct hash<dag::undirected_hyperedge<VertexType>> {
    uint64_t
    operator()(
        const dag::undirected_hyperedge<VertexType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::directed_hyperedge<VertexType>>{}(e), seed);
    }
  };
}  // namespace hll


namespace dag {
  // properties of directed hyperedge:

  template <network_vertex VertexType>
  directed_hyperedge<VertexType>::directed_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads) :
    directed_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads)) {}

  template <network_vertex VertexType>
  template <std::ranges::forward_range R1, std::ranges::forward_range R2>
  directed_hyperedge<VertexType>::directed_hyperedge(
      const R1& tails, const R2& heads) : _tails(tails), _heads(heads) {
    std::ranges::sort(_heads);
    auto [hfirst, hlast] = std::ranges::unique(_heads);
    _heads.erase(hfirst, hlast);

    std::ranges::sort(_tails);
    auto [tfirst, tlast] = std::ranges::unique(_tails);
    _tails.erase(tfirst, tlast);
  }

  template <network_vertex VertexType>
  inline bool
  directed_hyperedge<VertexType>::is_out_incident(const VertexType vert) const {
    return std::binary_search(_tails.begin(), _tails.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_hyperedge<VertexType>::is_in_incident(const VertexType vert) const  {
    return std::binary_search(_heads.begin(), _heads.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_hyperedge<VertexType>::is_incident(const VertexType vert) const {
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
  bool operator!=(
      const directed_hyperedge<VertexType>& a,
      const directed_hyperedge<VertexType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType>
  bool operator==(
      const directed_hyperedge<VertexType>& a,
      const directed_hyperedge<VertexType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <network_vertex VertexType>
  bool operator<(
      const directed_hyperedge<VertexType>& a,
      const directed_hyperedge<VertexType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <network_vertex VertexType>
  bool effect_lt(
      const directed_hyperedge<VertexType>& a,
      const directed_hyperedge<VertexType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
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

  template <network_vertex VertexType>
  inline std::tuple<std::vector<VertexType>, std::vector<VertexType>>
  directed_hyperedge<VertexType>::cause_comp_tuple() const {
    return std::make_tuple(_tails, _heads);
  }

  template <network_vertex VertexType>
  inline std::tuple<std::vector<VertexType>, std::vector<VertexType>>
  directed_hyperedge<VertexType>::effect_comp_tuple() const {
      return std::make_tuple(_heads, _tails);
  }


  // properties of undirected hyperedge:

  template <network_vertex VertexType>
  undirected_hyperedge<VertexType>::undirected_hyperedge(
      std::initializer_list<VertexType> verts) :
    undirected_hyperedge(std::vector<VertexType>(verts)) {}

  template <network_vertex VertexType>
  template <std::ranges::forward_range R>
  undirected_hyperedge<VertexType>::undirected_hyperedge(
      const R& verts) : _verts(verts) {
    std::ranges::sort(_verts);
    auto [first, last] = std::ranges::unique(_verts);
    _verts.erase(first, last);
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_incident(
      const VertexType vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }


  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_in_incident(
      const VertexType vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_hyperedge<VertexType>::is_out_incident(
      const VertexType vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
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
  bool operator!=(
      const undirected_hyperedge<VertexType>& a,
      const undirected_hyperedge<VertexType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType>
  bool operator==(
      const undirected_hyperedge<VertexType>& a,
      const undirected_hyperedge<VertexType>& b) {
    return (a._verts == b._verts);
  }

  template <network_vertex VertexType>
  bool operator<(
      const undirected_hyperedge<VertexType>& a,
      const undirected_hyperedge<VertexType>& b) {
      return (a._verts < b._verts);
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
}  // namespace dag
