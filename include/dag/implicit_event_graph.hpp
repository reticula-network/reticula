#ifndef INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_
#define INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_

#include <utility>
#include <vector>
#include <unordered_map>

#include "temporal_edges.hpp"
#include "networks.hpp"

namespace dag {
  namespace adjacency_prob {
    template <class EdgeT>
    class deterministic {
    public:
      explicit deterministic(typename EdgeT::TimeType dt);
      double p(const EdgeT& a, const EdgeT& b) const;
    private:
      typename EdgeT::TimeType _dt;
    };


    template <class EdgeT>
    class exponential {
    public:
      explicit exponential(typename EdgeT::TimeType expected_dt);
      double p(const EdgeT& a, const EdgeT& b) const;
    private:
      typename EdgeT::TimeType _dt;
    };
  }  // namespace adjacency_prob


  template <class EdgeT, class AdjacencyProbT>
  class implicit_event_graph {
  public:
    using TimeType = typename EdgeT::TimeType;
    using VertexType = typename EdgeT::VertexType;

    implicit_event_graph() = default;
    implicit_event_graph(
        const std::vector<EdgeT>& events,
        const AdjacencyProbT& prob,
        size_t seed);
    implicit_event_graph(
        const network<EdgeT>& temp,
        const AdjacencyProbT& prob,
        size_t seed);

    const std::vector<EdgeT>& events() const;
    std::pair<TimeType, TimeType> time_window() const;

    std::vector<EdgeT>
    predecessors(const EdgeT& e, bool just_first = false) const;

    std::vector<EdgeT>
    successors(const EdgeT& e, bool just_first = false) const;

  private:
    size_t _seed;
    network<EdgeT> _temp;
    AdjacencyProbT _prob;

    bool bernoulli_trial(const EdgeT& a, const EdgeT& b, double p) const;

    std::vector<EdgeT>
    successors_vert(const EdgeT& e, VertexType v, bool just_first) const;

    std::vector<EdgeT>
    predecessors_vert(const EdgeT& e, VertexType v, bool just_first) const;

    // Only enable deterministic shortcut (i.e. arXiv 1908.11831 appendix B) for
    // undirected temporal edges
    static constexpr bool enable_deterministic_shortcut = std::is_same<
      EdgeT, dag::undirected_temporal_edge<VertexType, TimeType>>::value;

    static constexpr bool is_prob_deterministic = std::is_same<
      AdjacencyProbT, dag::adjacency_prob::deterministic<EdgeT>>::value;
  };

}  // namespace dag

#include "../../src/implicit_event_graph.tpp"

#endif  // INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_
