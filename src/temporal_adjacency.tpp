#include <cmath>
#include <random>
#include <limits>

#include "../include/dag/utils.hpp"

namespace dag {
  namespace temporal_adjacency {
    template <temporal_edge EdgeT>
    typename EdgeT::TimeType
    simple<EdgeT>::cutoff_time(const EdgeT&) const {
      if constexpr (std::numeric_limits<typename EdgeT::TimeType>::has_infinity)
        return std::numeric_limits<typename EdgeT::TimeType>::infinity();
      else
        return std::numeric_limits<typename EdgeT::TimeType>::max();
    }

    template <temporal_edge EdgeT>
    double simple<EdgeT>::cutoff_dt(const EdgeT& e) const {
      return std::numeric_limits<typename EdgeT::TimeType>::infinity();
    }


    template <temporal_edge EdgeT>
    limited_waiting_time<EdgeT>::limited_waiting_time(
        typename EdgeT::TimeType dt) : _dt(dt) {}

    template <temporal_edge EdgeT>
    typename EdgeT::TimeType
    limited_waiting_time<EdgeT>::cutoff_time(const EdgeT& e) const {
      return e.effect_time() + _dt;
    }

    template <temporal_edge EdgeT>
    double limited_waiting_time<EdgeT>::cutoff_dt(const EdgeT& e) const {
      return static_cast<double>(_dt);
    }

    template <temporal_edge EdgeT>
    typename EdgeT::TimeType
    limited_waiting_time<EdgeT>::dt() const { return _dt; }

    template <temporal_edge EdgeT>
    exponential<EdgeT>::exponential(
        typename EdgeT::TimeType expected_dt, std::size_t seed) :
      _dt(expected_dt), _seed(seed) {}

    template <temporal_edge EdgeT>
    typename EdgeT::TimeType
    exponential<EdgeT>::cutoff_time(const EdgeT& e) const {
      double alive_dt = cutoff_dt(e);
      if constexpr (std::numeric_limits<typename EdgeT::TimeType>::is_integer)
        return e.effect_time() +
          static_cast<typename EdgeT::TimeType>(std::round(alive_dt));
      else
        return e.effect_time() +
          static_cast<typename EdgeT::TimeType>(alive_dt);
    }

    template <temporal_edge EdgeT>
    double exponential<EdgeT>::cutoff_dt(const EdgeT& e) const {
      size_t seed = utils::combine_hash<EdgeT, hash>(_seed, e);
      std::mt19937_64 gen(seed);
      std::exponential_distribution<double> dst(1.0/static_cast<double>(_dt));
      return dst(gen);
    }

    template <temporal_edge EdgeT>
    typename EdgeT::TimeType
    exponential<EdgeT>::expected_dt() const { return _dt; }
  }  // namespace temporal_adjacency
}  // namespace dag
