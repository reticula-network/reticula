#include <cmath>
#include <random>
#include <limits>

#include "../include/reticula/utils.hpp"

namespace reticula {
  namespace temporal_adjacency {
    // simple adjacency
    template <temporal_network_edge EdgeT>
    typename EdgeT::TimeType
    simple<EdgeT>::linger(
        const EdgeT&, const typename EdgeT::VertexType& v) const {
      return maximum_linger(v);
    }

    template <temporal_network_edge EdgeT>
    bool
    simple<EdgeT>::infinite_linger(
        const EdgeT&, const typename EdgeT::VertexType&) const {
      return true;
    }

    template <temporal_network_edge EdgeT>
    typename EdgeT::TimeType
    simple<EdgeT>::maximum_linger(
        const typename EdgeT::VertexType&) const {
      if constexpr (std::numeric_limits<typename EdgeT::TimeType>::has_infinity)
        return std::numeric_limits<typename EdgeT::TimeType>::infinity();
      else
        return std::numeric_limits<typename EdgeT::TimeType>::max();
    }


    // limited waiting-time adjacency
    template <temporal_network_edge EdgeT>
    limited_waiting_time<EdgeT>::limited_waiting_time(
        typename EdgeT::TimeType dt) : _dt(dt) {}

    template <temporal_network_edge EdgeT>
    typename EdgeT::TimeType
    limited_waiting_time<EdgeT>::linger(
        const EdgeT&, const typename EdgeT::VertexType&) const {
      return _dt;
    }

    template <temporal_network_edge EdgeT>
    bool
    limited_waiting_time<EdgeT>::infinite_linger(
        const EdgeT&, const typename EdgeT::VertexType&) const {
      if constexpr (std::numeric_limits<typename EdgeT::TimeType>::has_infinity)
        if (std::isinf(_dt))
          return true;
      return false;
    }

    template <temporal_network_edge EdgeT>
    typename EdgeT::TimeType
    limited_waiting_time<EdgeT>::maximum_linger(
        const typename EdgeT::VertexType&) const {
      return _dt;
    }

    template <temporal_network_edge EdgeT>
    typename EdgeT::TimeType
    limited_waiting_time<EdgeT>::dt() const {
      return _dt;
    }


    // exponential adjacency
    template <temporal_network_edge EdgeT>
    requires std::floating_point<typename EdgeT::TimeType>
    exponential<EdgeT>::exponential(
        typename EdgeT::TimeType rate, std::size_t seed) :
      _rate(rate), _seed(seed) {}

    template <temporal_network_edge EdgeT>
    requires std::floating_point<typename EdgeT::TimeType>
    typename EdgeT::TimeType
    exponential<EdgeT>::linger(
        const EdgeT& e, const typename EdgeT::VertexType& v) const {
      std::exponential_distribution<typename EdgeType::TimeType> dist(_rate);
      size_t seed = utils::combine_hash<EdgeT, hash>(_seed, e);
      seed = utils::combine_hash<typename EdgeT::VertexType, hash>(seed, v);
      std::mt19937_64 gen(seed);
      return dist(gen);
    }

    template <temporal_network_edge EdgeT>
    requires std::floating_point<typename EdgeT::TimeType>
    bool
    exponential<EdgeT>::infinite_linger(
        const EdgeT&, const typename EdgeT::VertexType&) const {
      return false;
    }

    template <temporal_network_edge EdgeT>
    requires std::floating_point<typename EdgeT::TimeType>
    typename EdgeT::TimeType
    exponential<EdgeT>::maximum_linger(
        const typename EdgeT::VertexType&) const {
      std::exponential_distribution<typename EdgeType::TimeType> dist(_rate);
      return dist.max();
    }

    template <temporal_network_edge EdgeT>
    requires std::floating_point<typename EdgeT::TimeType>
    typename EdgeT::TimeType exponential<EdgeT>::rate() const {
      return _rate;
    }


    // geometric adjacency
    template <temporal_network_edge EdgeT>
    requires std::integral<typename EdgeT::TimeType>
    geometric<EdgeT>::geometric(double p, std::size_t seed) :
      _p(p), _seed(seed) {}

    template <temporal_network_edge EdgeT>
    requires std::integral<typename EdgeT::TimeType>
    typename EdgeT::TimeType
    geometric<EdgeT>::linger(
        const EdgeT& e, const typename EdgeT::VertexType& v) const {
      std::geometric_distribution<typename EdgeType::TimeType> dist(_p);
      size_t seed = utils::combine_hash<EdgeT, hash>(_seed, e);
      seed = utils::combine_hash<typename EdgeT::VertexType, hash>(seed, v);
      std::mt19937_64 gen(seed);
      return dist(gen);
    }

    template <temporal_network_edge EdgeT>
    requires std::integral<typename EdgeT::TimeType>
    bool
    geometric<EdgeT>::infinite_linger(
        const EdgeT&, const typename EdgeT::VertexType&) const {
      return false;
    }

    template <temporal_network_edge EdgeT>
    requires std::integral<typename EdgeT::TimeType>
    typename EdgeT::TimeType
    geometric<EdgeT>::maximum_linger(
        const typename EdgeT::VertexType&) const {
      std::geometric_distribution<typename EdgeType::TimeType> dist(_p);
      return dist.max();
    }

    template <temporal_network_edge EdgeT>
    requires std::integral<typename EdgeT::TimeType>
    double geometric<EdgeT>::p() const {
      return _p;
    }
  }  // namespace temporal_adjacency
}  // namespace reticula
