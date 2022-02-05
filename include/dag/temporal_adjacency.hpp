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
      typename EdgeType::TimeType cutoff_time(const EdgeType& e) const;
      double cutoff_dt(const EdgeType& e) const;
    };

    template <temporal_edge EdgeT>
    class limited_waiting_time {
    public:
      using EdgeType = EdgeT;

      explicit limited_waiting_time(typename EdgeType::TimeType dt);
      typename EdgeType::TimeType cutoff_time(const EdgeType& e) const;
      double cutoff_dt(const EdgeType& e) const;

    private:
      typename EdgeType::TimeType _dt;
    };

    template <temporal_edge EdgeT>
    class exponential {
    public:
      using EdgeType = EdgeT;

      exponential(typename EdgeType::TimeType expected_dt, std::size_t seed);
      typename EdgeType::TimeType cutoff_time(const EdgeType& e) const;
      double cutoff_dt(const EdgeType& e) const;

    private:
      typename EdgeType::TimeType _dt;
      size_t _seed;
    };
  }  // namespace temporal_adjacency
}  // namespace dag

#include "../../src/temporal_adjacency.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_ADJACENCY_HPP_
