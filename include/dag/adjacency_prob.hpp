#ifndef INCLUDE_DAG_ADJACENCY_PROB_HPP_
#define INCLUDE_DAG_ADJACENCY_PROB_HPP_

namespace dag {
  namespace adjacency_prob {
    template <class EdgeT>
    class deterministic {
    public:
      /**
        Delta-t adjacency probability class. Two adjacent events `e1` and `e2`
        are delta-t adjacent if cause time of `e2` is less than delta-t apart
        from effect time of `e1`.
      */
      explicit deterministic(typename EdgeT::TimeType dt);

      /**
        Probability of events `a` and `b` being dt-adjacent if they are
        adjacent. If `a` and `b` are not adjacent calling this function will
        lead to undefined results.

        For deterministic adjacency probability, returns 1 if effect time of `a`
        and cause time of `b` are less than `dt` apart, otherwise 0.
       */
      double p(const EdgeT& a, const EdgeT& b) const;
      typename EdgeT::TimeType cutoff_dt() const;
    private:
      typename EdgeT::TimeType _dt;
    };


    template <class EdgeT>
    class exponential {
    public:
      /**
        Exponentially decaying adjacency probability class. Two adjacent events
        `e1` and `e2` are delta-t adjacent with probability decaying
        exponentially as a function of delta-t with expected value of
        `expected_dt`.
      */
      explicit exponential(typename EdgeT::TimeType expected_dt);

      /**
        Probability of events `a` and `b` being exponential-adjacent if they are
        adjacent. If `a` and `b` are not adjacent calling this function will
        lead to undefined results.

        For exponential adjacency probability, result value decreses
        exponentially based on the distance between effect time of `a` and cause
        time of `b` with an expected value of `expected_dt`.
       */
      double p(const EdgeT& a, const EdgeT& b) const;
    private:
      typename EdgeT::TimeType _dt;
      typename EdgeT::TimeType expected_dt() const;
    };

    /**
      Performs a bernoulli trial with probability `prob`, deterministic with
      seed derived from `seed`, `e1` and `e2`.
     */
    template <class EdgeT>
    bool
    bernoulli_trial(const EdgeT& a, const EdgeT& b, double prob, size_t seed);
  }  // namespace adjacency_prob
}  //namespace dag

#include "../../src/adjacency_prob.tpp"

#endif  // INCLUDE_DAG_ADJACENCY_PROB_HPP_
