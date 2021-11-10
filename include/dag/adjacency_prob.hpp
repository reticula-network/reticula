#ifndef INCLUDE_DAG_ADJACENCY_PROB_HPP_
#define INCLUDE_DAG_ADJACENCY_PROB_HPP_

#include "network_concepts.hpp"

namespace dag {
  namespace adjacency_prob {
    template <typename T>
    concept adjacency_prob = requires(
        T a, typename T::EdgeType e1, typename T::EdgeType e2) {
      { a.p(e1, e2) } -> std::convertible_to<double>;
      { a.cutoff_dt() } -> std::convertible_to<typename T::EdgeType::TimeType>;
    };  // NOLINT(readability/braces)

    template <dag::temporal_edge EdgeT>
    class deterministic {
    public:
      using EdgeType = EdgeT;

      /**
        Delta-t adjacency probability class. Two adjacent events `e1` and `e2`
        are delta-t adjacent if cause time of `e2` is less than delta-t apart
        from effect time of `e1`.
      */
      explicit deterministic(typename EdgeType::TimeType dt);

      /**
        Probability of events `a` and `b` being dt-adjacent if they are
        adjacent. If `a` and `b` are not adjacent calling this function will
        lead to undefined results.

        For deterministic adjacency probability, returns 1 if effect time of `a`
        and cause time of `b` are less than `dt` apart, otherwise 0.
       */
      double p(const EdgeType& a, const EdgeType& b) const;
      typename EdgeType::TimeType cutoff_dt() const;

    private:
      typename EdgeType::TimeType _dt;
    };


    template <dag::temporal_edge EdgeT>
    class exponential {
    public:
      using EdgeType = EdgeT;

      /**
        Exponentially decaying adjacency probability class. Two adjacent events
        `e1` and `e2` are delta-t adjacent with probability decaying
        exponentially as a function of delta-t with expected value of
        `expected_dt`.
      */
      explicit exponential(typename EdgeType::TimeType expected_dt);

      /**
        Probability of events `a` and `b` being exponential-adjacent if they are
        adjacent. If `a` and `b` are not adjacent calling this function will
        lead to undefined results.

        For exponential adjacency probability, result value decreses
        exponentially based on the distance between effect time of `a` and cause
        time of `b` with an expected value of `expected_dt`.
       */
      double p(const EdgeType& a, const EdgeType& b) const;
      typename EdgeType::TimeType cutoff_dt() const;

    private:
      typename EdgeType::TimeType _dt;
      typename EdgeType::TimeType expected_dt() const;
    };

    /**
      Performs a bernoulli trial with probability `prob`, deterministic with
      seed derived from `seed`, `e1` and `e2`.
     */
    template <temporal_edge EdgeT>
    bool
    bernoulli_trial(const EdgeT& a, const EdgeT& b, double prob, size_t seed);
  }  // namespace adjacency_prob
}  // namespace dag

#include "../../src/adjacency_prob.tpp"

#endif  // INCLUDE_DAG_ADJACENCY_PROB_HPP_
