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
      std::size_t verts_hash = std::reduce(
          e._verts.begin(), e._verts.end(), 0ul,
          [](std::size_t h, VertexType v) {
          return dag::utils::combine_hash<VertexType, hash>(h, v);});
      return dag::utils::combine_hash<TimeType, hash>(verts_hash, e._time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_temporal_hyperedge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::directed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::reduce(
          e._heads.begin(), e._heads.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
      std::size_t tails_hash = std::reduce(
          e._tails.begin(), e._tails.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
      return dag::utils::combine_hash<TimeType, hash>(
          dag::utils::combine_hash<std::size_t, hash>(
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
      std::size_t heads_hash = std::reduce(
          e._heads.begin(), e._heads.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
      std::size_t tails_hash = std::reduce(
          e._tails.begin(), e._tails.end(), 0ul,
          [](std::size_t h, VertexType v) {
            return dag::utils::combine_hash<VertexType, hash>(h, v);});
      return dag::utils::combine_hash<TimeType, hash>(
              dag::utils::combine_hash<TimeType, hash>(
                dag::utils::combine_hash<std::size_t, hash>(
                  heads_hash, tails_hash),
                e._time),
              e._delay);
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
  template <std::ranges::forward_range R1, std::ranges::forward_range R2>
  directed_temporal_hyperedge<VertexType, TimeType>::
    directed_temporal_hyperedge(
      const R1& tails, const R2& heads, TimeType time) :
      _tails(tails), _heads(heads), _time(time) {
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
  bool operator!=(
        const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator==(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator<(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
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

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, std::vector<VertexType>, std::vector<VertexType>>
  directed_temporal_hyperedge<VertexType, TimeType>::cause_comp_tuple() const {
    return std::make_tuple(_time, _tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, std::vector<VertexType>, std::vector<VertexType>>
  directed_temporal_hyperedge<VertexType, TimeType>::effect_comp_tuple() const {
    return std::make_tuple(_time, _heads, _tails);
  }


  // properties of directed delayed temporal edges:

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads,
      TimeType time, TimeType delay) :
    directed_delayed_temporal_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads),
        time, delay) {}

  template <network_vertex VertexType, typename TimeType>
  template <std::ranges::forward_range R1, std::ranges::forward_range R2>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      const R1& tails, const R2& heads,
      TimeType time, TimeType delay) :
    _tails(tails), _heads(heads), _time(time), _delay(delay) {
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
  directed_hyperedge<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      static_projection() const {
    return directed_hyperedge<VertexType>(_tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      effect_time() const {
    return _time + _delay;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      cause_time() const {
    return _time;
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
  bool operator!=(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator==(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator<(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._time + a._delay >= b._time) {
      return false;
    } else {
      std::vector<VertexType> common;
      std::ranges::set_intersection(a._heads, b._tails,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, TimeType,
    std::vector<VertexType>, std::vector<VertexType>>
    directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      cause_comp_tuple() const {
    return std::make_tuple(_time, _time+_delay, _tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, TimeType,
    std::vector<VertexType>, std::vector<VertexType>>
    directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      effect_comp_tuple() const {
    return std::make_tuple(_time+_delay, _time, _heads, _tails);
  }


  // properties of undirected temporal edges:

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      std::initializer_list<VertexType> verts, TimeType time) :
    undirected_temporal_hyperedge(std::vector<VertexType>(verts), time) {}

  template <network_vertex VertexType, typename TimeType>
  template <std::ranges::forward_range R>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      const R& verts, const TimeType time) : _verts(verts), _time(time) {
    std::ranges::sort(_verts);
    auto [first, last] = std::ranges::unique(_verts);
    _verts.erase(first, last);
    _verts.shrink_to_fit();
  }

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
  bool operator!=(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator==(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.comp_tuple() == b.comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator<(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.comp_tuple() < b.comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    return (a.comp_tuple() < b.comp_tuple());
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

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, std::vector<VertexType>>
  undirected_temporal_hyperedge<VertexType, TimeType>::comp_tuple() const {
    return std::make_tuple(_time, _verts);
  }
}  // namespace dag


