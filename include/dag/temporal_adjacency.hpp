#ifndef INCLUDE_DAG_TEMPORAL_ADJACENCY_HPP_
#define INCLUDE_DAG_TEMPORAL_ADJACENCY_HPP_

#include "network_concepts.hpp"

namespace dag {
  namespace temporal_adjacency {
    template <typename T>
    concept temporal_adjacency =
      temporal_edge<typename T::EdgeType> &&
      requires(T a, typename T::EdgeType e) {
        { a.cutoff_time(e) } -> std::convertible_to<
          typename T::EdgeType::TimeType>;
        { a.cutoff_dt(e) } -> std::convertible_to<double>;
      };  // NOLINT(readability/braces)

    template <temporal_edge EdgeT>
    class simple {
    public:
      using EdgeType = EdgeT;

      simple() = default;
      /**
        Last timestamp where an effect transmitted by event `e` will linger in
        the mutated vertices of that event. For simple temporal adjacency, this
        is equal to the largest value that can be represented by
        `EdgeType::TimeType`.

        @param e The transmitting event
      */
      typename EdgeType::TimeType cutoff_time(const EdgeType& e) const;
      /**
        Duration where an effect transmitted by event `e` will linger in the
        mutated vertices of that event. For simple temporal adjacency, this
        is equal to positive infinity.

        @param e The transmitting event
      */
      double cutoff_dt(const EdgeType& e) const;
    };

    template <temporal_edge EdgeT>
    class limited_waiting_time {
    public:
      using EdgeType = EdgeT;

      explicit limited_waiting_time(typename EdgeType::TimeType dt);
      /**
        Last timestamp where an effect transmitted by event `e` will linger in
        the mutated vertices of that event. For limited waiting-time temporal
        adjacency, this is equal to the effect time of `e` plus the parameter
        `dt`.

        @param e The transmitting event
      */
      typename EdgeType::TimeType cutoff_time(const EdgeType& e) const;
      /**
        Duration where an effect transmitted by event `e` will linger in the
        mutated vertices of that event. For limited waiting-time temporal
        adjacency, this is equal to the parameter `dt`.

        @param e The transmitting event
      */
      double cutoff_dt(const EdgeType& e) const;

      /**
        Returns the parameter `dt` of the the temporal adjacency. This is equal
        to the maximum waiting time and the `cutoff_dt` values.
      */
      typename EdgeType::TimeType dt() const;

    private:
      typename EdgeType::TimeType _dt;
    };

    template <temporal_edge EdgeT>
    class exponential {
    public:
      using EdgeType = EdgeT;

      exponential(typename EdgeType::TimeType expected_dt, std::size_t seed);
      /**
        Last timestamp (in units of time used in EdgeType) where an effect
        transmitted by event `e` will linger in the mutated vertices of that
        event. for exponential temporal adjacency, this is drawn from an
        exponential districution with mean of `expected_dt`, then rounded (if
        `EdgeType::TimeType` is an integer type) to the nearest timestamp.

        @param e The transmitting event
      */
      typename EdgeType::TimeType cutoff_time(const EdgeType& e) const;
      /**
        Duration where an effect transmitted by event `e` will linger in the
        mutated vertices of that event. for exponential temporal adjacency, this
        is drawn from an exponential districution with mean of `expected_dt`.

        @param e The transmitting event
      */
      double cutoff_dt(const EdgeType& e) const;

      /**
        Returns the parameter `expected_dt` of the the temporal adjacency. This
        is equal to the reciprocal of the rate of exponential distibution
        generating `cutoff_dt` and `cutoff_time` values, before the latter is
        rounded to the units of time used for `EdgeType`.
      */
      typename EdgeType::TimeType expected_dt() const;

    private:
      typename EdgeType::TimeType _dt;
      size_t _seed;
    };
  }  // namespace temporal_adjacency
}  // namespace dag

#include "../../src/temporal_adjacency.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_ADJACENCY_HPP_
