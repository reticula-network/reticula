#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

namespace std {
  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::undirected_temporal_hyperedge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::undirected_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t verts_hash = std::accumulate(
          e._verts.begin(), e._verts.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
          return dag::utils::combine_hash<VertexType, dag::hash>(h, v);});
      return dag::utils::combine_hash<TimeType, dag::hash>(verts_hash, e._time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_temporal_hyperedge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::directed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return dag::utils::combine_hash<VertexType, dag::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return dag::utils::combine_hash<VertexType, dag::hash>(h, v);});
      return dag::utils::combine_hash<TimeType, dag::hash>(
          dag::utils::combine_hash<std::size_t, dag::hash>(
            heads_hash, tails_hash), e._time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_delayed_temporal_hyperedge<VertexType, TimeType>> {
    size_t
    operator()(
        const
        dag::directed_delayed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return dag::utils::combine_hash<VertexType, dag::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return dag::utils::combine_hash<VertexType, dag::hash>(h, v);});
      return dag::utils::combine_hash<TimeType, dag::hash>(
              dag::utils::combine_hash<TimeType, dag::hash>(
                dag::utils::combine_hash<std::size_t, dag::hash>(
                  heads_hash, tails_hash),
                e._cause_time),
              e._effect_time);
    }
  };
}  // namespace std


namespace hll {
  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::undirected_temporal_hyperedge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::undirected_temporal_hyperedge<VertexType, TimeType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<
            dag::undirected_temporal_hyperedge<VertexType, TimeType>>{}(e),
          seed);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_temporal_hyperedge<VertexType, TimeType>> {
    uint64_t
    operator()(
        const dag::directed_temporal_hyperedge<VertexType, TimeType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<
            dag::directed_temporal_hyperedge<VertexType, TimeType>>{}(e),
          seed);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_delayed_temporal_hyperedge<VertexType, TimeType>> {
    uint64_t
    operator()(
        const dag::directed_delayed_temporal_hyperedge<VertexType, TimeType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<
            dag::directed_delayed_temporal_hyperedge<
              VertexType, TimeType>>{}(e),
          seed);
    }
  };
}  // namespace hll

namespace dag {
  // properties of directed temporal edge:

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_hyperedge<VertexType, TimeType>::
    directed_temporal_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads,
      TimeType time) :
    directed_temporal_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads), time) {}

  template <network_vertex VertexType, typename TimeType>
  template <std::ranges::input_range R1, std::ranges::input_range R2>
  requires
    std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::
    directed_temporal_hyperedge(
      const R1& tails, const R2& heads, TimeType time) : _time(time) {
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

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_hyperedge<VertexType, TimeType>::
  directed_temporal_hyperedge(
    const directed_hyperedge<VertexType>& projection, TimeType time)
    : _time(time), _tails(projection.tails()), _heads(projection.heads()) {}

  template <network_vertex VertexType, typename TimeType>
  directed_hyperedge<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::static_projection() const {
    return directed_hyperedge<VertexType>(_tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_hyperedge<VertexType, TimeType>::effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_hyperedge<VertexType, TimeType>::cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return std::ranges::binary_search(_tails, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return std::ranges::binary_search(_heads, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::mutator_verts() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::mutated_verts() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::incident_verts() const {
    std::vector<VertexType> res;
    res.reserve(_heads.size() + _tails.size());
    std::ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::tails() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::heads() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    return a < b;
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._time >= b._time) {
      return false;
    } else {
      std::vector<VertexType> common;
      std::ranges::set_intersection(a._heads, b._tails,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }

  // properties of directed delayed temporal edges:

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads,
      TimeType cause_time, TimeType effect_time) :
    directed_delayed_temporal_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads),
        cause_time, effect_time) {}

  template <network_vertex VertexType, typename TimeType>
  template <std::ranges::input_range R1, std::ranges::input_range R2>
  requires
    std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      const R1& tails, const R2& heads,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_hyperedge cannot"
          " have a cause_time larger than effect_time");

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

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      const directed_hyperedge<VertexType>& projection,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time),
      _tails(projection.tails()), _heads(projection.heads()) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_hyperedge cannot"
          " have a cause_time larger than effect_time");
  }


  template <network_vertex VertexType, typename TimeType>
  directed_hyperedge<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      static_projection() const {
    return directed_hyperedge<VertexType>(_tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      effect_time() const {
    return _effect_time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      cause_time() const {
    return _cause_time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_out_incident(const VertexType& vert) const {
    return std::ranges::binary_search(_tails, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_in_incident(const VertexType& vert) const {
    return std::ranges::binary_search(_heads, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_incident(const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      mutator_verts() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      mutated_verts() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      tails() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      heads() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      incident_verts() const {
    std::vector<VertexType> res;
    res.reserve(_heads.size() + _tails.size());
    std::ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    return std::make_tuple(a._effect_time, a._cause_time, a._heads, a._tails) <
      std::make_tuple(b._effect_time, b._cause_time, b._heads, b._tails);
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._effect_time >= b._cause_time) {
      return false;
    } else {
      std::vector<VertexType> common;
      std::ranges::set_intersection(a._heads, b._tails,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }

  // properties of undirected temporal edges:

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      std::initializer_list<VertexType> verts, TimeType time) :
    undirected_temporal_hyperedge(std::vector<VertexType>(verts), time) {}

  template <network_vertex VertexType, typename TimeType>
  template <std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      const R& verts, const TimeType time) : _time(time) {
    if constexpr (std::ranges::sized_range<R>)
      _verts.reserve(std::ranges::size(verts));
    std::ranges::copy(verts, std::back_inserter(_verts));
    std::ranges::sort(_verts);
    auto [first, last] = std::ranges::unique(_verts);
    _verts.erase(first, last);
    _verts.shrink_to_fit();
  }


  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
  undirected_temporal_hyperedge(
      const undirected_hyperedge<VertexType>& projection, TimeType time) :
    _time(time), _verts(projection.incident_verts()) {}

  template <network_vertex VertexType, typename TimeType>
  undirected_hyperedge<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
      static_projection() const {
    return undirected_hyperedge<VertexType>(_verts);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_hyperedge<VertexType, TimeType>::
      effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_hyperedge<VertexType, TimeType>::
      cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_incident(const VertexType& vert) const {
    return std::ranges::binary_search(_verts, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_in_incident(const VertexType& vert) const {
    return std::ranges::binary_search(_verts, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_out_incident(const VertexType& vert) const {
    return std::ranges::binary_search(_verts, vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::mutator_verts() const {
    return _verts;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::mutated_verts() const {
    return _verts;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::incident_verts() const {
    return _verts;
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    return a < b;
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._time >= b._time) {
      return false;
    } else {
      std::vector<VertexType> common;
      std::ranges::set_intersection(a._verts, b._verts,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }
}  // namespace dag
