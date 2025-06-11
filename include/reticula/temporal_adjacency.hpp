#ifndef INCLUDE_RETICULA_TEMPORAL_ADJACENCY_HPP_
#define INCLUDE_RETICULA_TEMPORAL_ADJACENCY_HPP_

#include <concepts>

#include "network_concepts.hpp"

namespace reticula {
  namespace temporal_adjacency {
    template <typename T>
    concept temporal_adjacency =
      temporal_network_edge<typename T::EdgeType> &&
      requires(
          T a,
          typename T::EdgeType e,
          typename T::VertexType v) {
        { a.linger(e, v) } -> std::convertible_to<
          typename T::EdgeType::TimeType>;
        { a.infinite_linger(e, v) } -> std::convertible_to<bool>;
        { a.maximum_linger(v) } -> std::convertible_to<
          typename T::EdgeType::TimeType>;
      };  // NOLINT(readability/braces)

    template <temporal_network_edge EdgeT>
    class simple {
    public:
      using EdgeType = EdgeT;
      using VertexType = typename EdgeT::VertexType;

      simple() = default;
      /**
        Duration where an effect transmitted by event `e` will linger in the
        mutated vertex `v` of that event. For simple temporal adjacency, this is
        equal to the largest value that can be represented by
        `EdgeType::TimeType`.

        @param e The transmitting event
        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      linger(const EdgeType& e, const VertexType& v) const;

      /**
        Determines whether the effect transmitted by event `e` lingers in the
        mutated vertex `v` forever, regardless of the fact that
        EdgeType::TimeType might not support infinity.

        Returns true for all simple adjacency instances.

        @param e The transmitting event
        @param v The mutated vertex
      */
      bool
      infinite_linger(const EdgeType& e, const VertexType& v) const;

      /**
        Maximum theoritical duration where an effect transmitted by an event
        will linger in a mutated vertex `v`. For simple temporal adjacency, this
        is equal to the maximum value that can be represented by the time type.
        For simple temporal adjacency, this is equal to the largest value that
        can be represented by `EdgeType::TimeType`.

        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      maximum_linger(const VertexType& v) const;
    };

    template <temporal_network_edge EdgeT>
    class limited_waiting_time {
    public:
      using EdgeType = EdgeT;
      using VertexType = typename EdgeT::VertexType;

      explicit limited_waiting_time(typename EdgeType::TimeType dt);

      /**
        Duration where an effect transmitted by event `e` will linger in the
        mutated vertex `v` of that event. For the limited waiting-time temporal
        adjacency, this is equal to the parameter `dt`.

        @param e The transmitting event
        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      linger(const EdgeType& e, const VertexType& v) const;

      /**
        Determines whether the effect transmitted by event `e` lingers in the
        mutated vertex `v` forever, regardless of the fact that
        EdgeType::TimeType might not support infinity.

        Returns true if the limited waiting time is infinite, otherwise returns
        false.

        @param e The transmitting event
        @param v The mutated vertex
      */
      bool
      infinite_linger(const EdgeType& e, const VertexType& v) const;

      /**
        Maximum theoritical duration where an effect transmitted by an event
        will linger in a mutated vertex `v`. For the limited waiting-time temporal
        adjacency, this is equal to the parameter `dt`.

        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      maximum_linger(const VertexType& v) const;

      /**
        Returns the parameter `dt` of the the temporal adjacency.
      */
      typename EdgeType::TimeType dt() const;

    private:
      typename EdgeType::TimeType _dt;
    };

    template <temporal_network_edge EdgeT>
    requires std::floating_point<typename EdgeT::TimeType>
    class exponential {
    public:
      using EdgeType = EdgeT;
      using VertexType = typename EdgeT::VertexType;

      exponential(typename EdgeT::TimeType rate, std::size_t seed);

      /**
        Duration where an effect transmitted by event `e` will linger in the
        mutated vertex `v` of that event. For the exponential temporal
        adjacency, this is drawn from an exponential distribution with rate
        `rate`.

        @param e The transmitting event
        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      linger(const EdgeType& e, const VertexType& v) const;

      /**
        Determines whether the effect transmitted by event `e` lingers in the
        mutated vertex `v` forever, regardless of the fact that
        EdgeType::TimeType might not support infinity.

        Returns false for all exponential adjacency instances.

        @param e The transmitting event
        @param v The mutated vertex
      */
      bool
      infinite_linger(const EdgeType& e, const VertexType& v) const;

      /**
        Maximum theoritical duration where an effect transmitted by an event
        will linger in a mutated vertex `v`. For the exponential temporal
        adjacency, this is is equal to the largest value that can be represented
        in `EdgeType::TimeType`.

        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      maximum_linger(const VertexType& v) const;

      /**
        Returns the parameter `rate` of the the temporal adjacency. This
        is equal to the reciprocal of the mean of exponential distibution
        generating `linger` values.
      */
      typename EdgeT::TimeType rate() const;

    private:
      typename EdgeType::TimeType _rate;
      size_t _seed;
    };


    template <temporal_network_edge EdgeT>
    requires std::integral<typename EdgeT::TimeType>
    class geometric {
    public:
      using EdgeType = EdgeT;
      using VertexType = typename EdgeT::VertexType;

      geometric(double p, std::size_t seed);

      /**
        Duration (inclusive) where an effect transmitted by event `e` will
        linger in the mutated vertex `v` of that event. For the exponential
        temporal adjacency, this is drawn from a geometric distribution with
        parameter `p`.

        @param e The transmitting event
        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      linger(const EdgeType& e, const VertexType& v) const;

      /**
        Determines whether the effect transmitted by event `e` lingers in the
        mutated vertex `v` forever, regardless of the fact that
        EdgeType::TimeType might not support infinity.

        Returns false for all geometric adjacency instances.

        @param e The transmitting event
        @param v The mutated vertex
      */
      bool
      infinite_linger(const EdgeType& e, const VertexType& v) const;

      /**
        Maximum theoritical duration where an effect transmitted by an event
        will linger in a mutated vertex `v`. For the geometric temporal
        adjacency, this is is equal to the largest value that can be represented
        in `EdgeType::TimeType`.

        @param v The mutated vertex
      */
      typename EdgeType::TimeType
      maximum_linger(const VertexType& v) const;

      /**
        Returns the parameter `p` of the the temporal adjacency. This is equal
        to the probability of lingering of the effect at each tick of the time.
        The mean value of `linger` that will generated is equal to 1/p - 1.
      */
      [[nodiscard]] double p() const;

    private:
      double _p;
      size_t _seed;
    };
  }  // namespace temporal_adjacency
}  // namespace reticula

// Implementation
#include <cmath>
#include <random>
#include <limits>

#include "utils.hpp"

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


#endif  // INCLUDE_RETICULA_TEMPORAL_ADJACENCY_HPP_
