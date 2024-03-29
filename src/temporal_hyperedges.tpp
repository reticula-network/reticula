#include <functional>
#include <algorithm>
#include <istream>
#include <ostream>

#include "../include/reticula/utils.hpp"

namespace std {
  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::undirected_temporal_hyperedge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::undirected_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t verts_hash = std::accumulate(
          e._verts.begin(), e._verts.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
          return reticula::utils::combine_hash<
            VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<
        TimeType, reticula::hash>(verts_hash, e._time);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_temporal_hyperedge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::directed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
          reticula::utils::combine_hash<std::size_t, reticula::hash>(
            heads_hash, tails_hash), e._time);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_delayed_temporal_hyperedge<
      VertexType, TimeType>> {
    std::size_t
    operator()(
        const
        reticula::directed_delayed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
              reticula::utils::combine_hash<TimeType, reticula::hash>(
                reticula::utils::combine_hash<std::size_t, reticula::hash>(
                  heads_hash, tails_hash),
                e._cause_time),
              e._effect_time);
    }
  };
}  // namespace std


namespace hll {
  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::undirected_temporal_hyperedge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::undirected_temporal_hyperedge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<reticula::undirected_temporal_hyperedge<
            VertexType, TimeType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_temporal_hyperedge<VertexType, TimeType>> {
    uint64_t
    operator()(
        const reticula::directed_temporal_hyperedge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<reticula::directed_temporal_hyperedge<
            VertexType, TimeType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_delayed_temporal_hyperedge<
      VertexType, TimeType>> {
    uint64_t
    operator()(
        const reticula::directed_delayed_temporal_hyperedge<
          VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<reticula::directed_delayed_temporal_hyperedge<
              VertexType, TimeType>>{}(e), seed);
    }
  };
}  // namespace hll

namespace reticula {
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
  template <ranges::input_range R1, ranges::input_range R2>
  requires
    std::convertible_to<ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<ranges::range_value_t<R2>, VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::
    directed_temporal_hyperedge(
      const R1& tails, const R2& heads, TimeType time) : _time(time) {
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
    return std::binary_search(_tails.begin(), _tails.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return std::binary_search(_heads.begin(), _heads.end(), vert);
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
    ranges::set_union(_tails, _heads, std::back_inserter(res));
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

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto directed_temporal_hyperedge<VertexType, TimeType>::operator<=>(
      const directed_temporal_hyperedge<VertexType, TimeType>& o) const {
    return utils::compare(std::tie(_time, _tails, _heads),
                          std::tie(o._time, o._tails, o._heads));
  }
#endif


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
      ranges::set_intersection(a._heads, b._tails,
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
  template <ranges::input_range R1, ranges::input_range R2>
  requires
    std::convertible_to<ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<ranges::range_value_t<R2>, VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      const R1& tails, const R2& heads,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_hyperedge cannot"
          " have a cause_time larger than effect_time");

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
    return std::binary_search(_tails.begin(), _tails.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_in_incident(const VertexType& vert) const {
    return std::binary_search(_heads.begin(), _heads.end(), vert);
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
    ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto directed_delayed_temporal_hyperedge<VertexType, TimeType>::operator<=>(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& o)
      const {
    return utils::compare(
      std::tie(_cause_time, _effect_time, _tails, _heads),
      std::tie(o._cause_time, o._effect_time, o._tails, o._heads));
  }
#endif

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
      ranges::set_intersection(a._heads, b._tails,
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
  template <ranges::input_range R>
  requires std::convertible_to<ranges::range_value_t<R>, VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      const R& verts, const TimeType time) : _time(time) {
    if constexpr (ranges::sized_range<R>)
      _verts.reserve(ranges::size(verts));
    ranges::copy(verts, std::back_inserter(_verts));
    ranges::sort(_verts);
    auto [first, last] = ranges::unique(_verts);
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
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_in_incident(const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_out_incident(const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
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

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto undirected_temporal_hyperedge<VertexType, TimeType>::operator<=>(
      const undirected_temporal_hyperedge<VertexType, TimeType>& o) const {
    return utils::compare(std::tie(_time, _verts), std::tie(o._time, o._verts));
  }
#endif

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
      ranges::set_intersection(a._verts, b._verts,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }
}  // namespace reticula
