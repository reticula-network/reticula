#ifndef INCLUDE_RETICULA_TEMPORAL_ADJACENCY_HPP_
#define INCLUDE_RETICULA_TEMPORAL_ADJACENCY_HPP_

#include <concepts>

#include "network_concepts.hpp"

namespace reticula {
  namespace temporal_adjacency {
    template <typename T>
    concept temporal_adjacency =
      temporal_edge<typename T::EdgeType> &&
      requires(
          T a,
          typename T::EdgeType e,
          typename T::VertexType v) {
        { a.linger(e, v) } -> std::convertible_to<
          typename T::EdgeType::TimeType>;
        { a.maximum_linger(v) } -> std::convertible_to<
          typename T::EdgeType::TimeType>;
      };  // NOLINT(readability/braces)

    template <temporal_edge EdgeT>
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

    template <temporal_edge EdgeT>
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

    template <temporal_edge EdgeT>
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


    template <temporal_edge EdgeT>
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
      double p() const;

    private:
      double _p;
      size_t _seed;
    };
  }  // namespace temporal_adjacency
}  // namespace reticula

#include "../../src/temporal_adjacency.tpp"

#endif  // INCLUDE_RETICULA_TEMPORAL_ADJACENCY_HPP_
