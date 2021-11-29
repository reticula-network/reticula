#include <unordered_map>
#include <ds/disjoint_set.hpp>

#include "../include/dag/type_traits.hpp"

namespace dag {
  /**
    Only enable reduced event graph (i.e. arXiv 1908.11831 appendix B) for
    undirected temporal edges
   */
  template <class EventGraph>
  struct is_reducable {
    static constexpr bool value = false;
  };

  template <class EventGraph>
  inline constexpr bool
  is_reducable_v = is_reducable<EventGraph>::value;


  template <temporal_edge EdgeT>
  struct is_reducable<
    dag::implicit_event_graph<EdgeT,
      dag::adjacency_prob::deterministic<EdgeT>>> {
    static constexpr bool value = dag::is_undirected_v<EdgeT>;
  };

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT,
           template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    temporal_component<EdgeT, ReadOnlyEstimatorT>>>
  out_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      std::size_t seed,
      bool only_roots) {
    std::unordered_map<EdgeT, temporal_component<EdgeT, EstimatorT>>
      out_components;
    std::vector<
      std::pair<EdgeT, temporal_component<EdgeT, ReadOnlyEstimatorT>>>
        out_component_ests;
    out_component_ests.reserve(eg.events_cause().size());

    std::unordered_map<EdgeT, std::size_t> in_degrees;

    auto temp_edge_iter = eg.events_cause().rbegin();
    auto end = eg.events_cause().rend();

    constexpr bool reducable = is_reducable_v<
      implicit_event_graph<EdgeT, AdjacencyProbT>>;

    while (temp_edge_iter < end) {
      out_components.emplace(*temp_edge_iter, seed);

      std::vector<EdgeT> successors = eg.successors(
          *temp_edge_iter, reducable);
      std::vector<EdgeT> predecessors = eg.predecessors(
          *temp_edge_iter, reducable);

      in_degrees[*temp_edge_iter] = predecessors.size();

      for (const auto& other: successors) {
        out_components.at(*temp_edge_iter).merge(out_components.at(other));

        if (--in_degrees.at(other) == 0) {
          if (!only_roots)
            out_component_ests.emplace_back(other,
                out_components.at(other));
          out_components.erase(other);
          in_degrees.erase(other);
        }
      }

      auto nodes = temp_edge_iter->mutated_verts();
      out_components.at(*temp_edge_iter).insert(*temp_edge_iter, nodes);

      if (in_degrees.at(*temp_edge_iter) == 0) {
        out_component_ests.emplace_back(*temp_edge_iter,
          out_components.at(*temp_edge_iter));
        out_components.erase(*temp_edge_iter);
        in_degrees.erase(*temp_edge_iter);
      }
      temp_edge_iter++;
    }

    if (only_roots)
      out_component_ests.shrink_to_fit();

    return out_component_ests;
  }

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT,
           template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    temporal_component<EdgeT, ReadOnlyEstimatorT>>>
  in_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      std::size_t seed,
      bool only_roots) {
    std::unordered_map<EdgeT, temporal_component<EdgeT, EstimatorT>>
      in_components;
    std::vector<
      std::pair<EdgeT, temporal_component<EdgeT, ReadOnlyEstimatorT>>>
        in_component_ests;
    in_component_ests.reserve(eg.events_cause().size());

    std::unordered_map<EdgeT, std::size_t> out_degrees;

    auto temp_edge_iter = eg.events_effect().begin();
    auto end = eg.events_effect().end();


    constexpr bool reducable = is_reducable_v<
      implicit_event_graph<EdgeT, AdjacencyProbT>>;

    while (temp_edge_iter < end) {
      in_components.emplace(*temp_edge_iter, seed);

      std::vector<EdgeT> successors = eg.successors(
          *temp_edge_iter, reducable);
      std::vector<EdgeT> predecessors = eg.predecessors(
          *temp_edge_iter, reducable);

      out_degrees[*temp_edge_iter] = successors.size();

      for (const auto& other: predecessors) {
        in_components.at(*temp_edge_iter).merge(in_components.at(other));

        if (--out_degrees.at(other) == 0) {
          if (!only_roots)
            in_component_ests.emplace_back(other,
                in_components.at(other));
          in_components.erase(other);
          out_degrees.erase(other);
        }
      }

      auto nodes = temp_edge_iter->mutator_verts();
      in_components.at(*temp_edge_iter).insert(*temp_edge_iter, nodes);

      if (out_degrees.at(*temp_edge_iter) == 0) {
        in_component_ests.emplace_back(*temp_edge_iter,
          in_components.at(*temp_edge_iter));
        in_components.erase(*temp_edge_iter);
        out_degrees.erase(*temp_edge_iter);
      }
      temp_edge_iter++;
    }

    if (only_roots)
      in_component_ests.shrink_to_fit();

    return in_component_ests;
  }

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _generic_out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t seed,
      std::size_t node_size_est,
      std::size_t edge_size_est,
      bool revert_graph) {
    std::queue<EdgeT> search({root});
    temporal_component<EdgeT, EstimatorT>
      out_component(seed, edge_size_est, node_size_est);
    auto nodes = (revert_graph ?
      root.mutator_verts() : root.mutated_verts());
    out_component.insert(root, nodes);

    constexpr bool reducable = is_reducable_v<
      implicit_event_graph<EdgeT, AdjacencyProbT>>;

    while (!search.empty()) {
      EdgeT e = search.front();
      search.pop();
      auto next_events = (revert_graph ?
          eg.predecessors(e, reducable) :
          eg.successors(e, reducable));
      for (auto&& s: next_events)
        if (!out_component.edge_set().contains(s)) {
          search.push(s);
          auto nodes = (revert_graph ?
              s.mutator_verts() : s.mutated_verts());
          out_component.insert(s, nodes);
        }
    }

    return out_component;
  }


  template <temporal_edge EdgeT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _deterministic_out_component(
      const implicit_event_graph<EdgeT,
        adjacency_prob::deterministic<EdgeT>>& eg,
      const EdgeT& root,
      std::size_t seed,
      std::size_t node_size_est,
      std::size_t edge_size_est) {
    auto comp_function = [](const EdgeT& e1, const EdgeT& e2) {
      return (e1.effect_time()) > (e2.effect_time());
    };

    std::priority_queue<
      EdgeT, std::vector<EdgeT>,
      decltype(comp_function)> in_transition(comp_function);

    in_transition.push(root);

    using VertexType = typename EdgeT::VertexType;
    using TimeType = typename EdgeT::TimeType;

    adjacency_prob::deterministic<EdgeT> prob = eg.adjacency_prob();

    temporal_component<EdgeT, EstimatorT>
      out_component(seed, edge_size_est, node_size_est);
    out_component.insert(root, root.mutated_verts());

    std::unordered_map<VertexType, TimeType> last_infected;
    last_infected.reserve(eg.events_cause().size());

    for (auto && v: root.mutated_verts())
      last_infected[v] = root.effect_time();

    TimeType last_infect_time = root.effect_time();

    auto begin = eg.events_cause().begin();
    auto end = eg.events_cause().end();

    auto topo_it = std::upper_bound(begin, end, root);

    while (topo_it < end &&
        (topo_it->cause_time() < last_infect_time ||
        topo_it->cause_time() - last_infect_time < prob.cutoff_dt())) {
      while (!in_transition.empty() &&
          in_transition.top().effect_time() < topo_it->cause_time()) {
        for (auto && v: in_transition.top().mutated_verts()) {
          last_infected[v] = in_transition.top().effect_time();
        }

        auto event = in_transition.top();
        out_component.insert(event, event.mutated_verts());
        in_transition.pop();
      }


      bool is_infecting = false;

      for (auto && v: topo_it->mutator_verts()) {
        auto last_infected_it = last_infected.find(v);
        if (last_infected_it != last_infected.end() &&
            topo_it->cause_time() > last_infected_it->second &&
            topo_it->cause_time() - last_infected_it->second < prob.cutoff_dt())
          is_infecting = true;
      }

      if (is_infecting) {
        if (topo_it->cause_time() == topo_it->effect_time()) {
          out_component.insert(*topo_it, topo_it->mutated_verts());
          for (auto && v: topo_it->mutated_verts())
            last_infected[v] = topo_it->cause_time();
        } else {
          in_transition.push(*topo_it);
        }
        last_infect_time =
          std::max(topo_it->effect_time(), last_infect_time);
      }

      topo_it++;
    }

    while (!in_transition.empty()) {
      auto event = in_transition.top();
      out_component.insert(event, event.mutated_verts());
      in_transition.pop();
    }

    return out_component;
  }


  template <temporal_edge EdgeT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _deterministic_in_component(
      const implicit_event_graph<EdgeT,
          adjacency_prob::deterministic<EdgeT>>& eg,
      const EdgeT& root,
      std::size_t seed,
      std::size_t node_size_est,
      std::size_t edge_size_est) {
    auto comp_function = [](const EdgeT& e1, const EdgeT& e2) {
      return (e1.cause_time()) < (e2.cause_time());
    };

    std::priority_queue<
      EdgeT, std::vector<EdgeT>,
      decltype(comp_function)> in_transition(comp_function);

    in_transition.push(root);

    using VertexType = typename EdgeT::VertexType;
    using TimeType = typename EdgeT::TimeType;

    adjacency_prob::deterministic<EdgeT> prob = eg.adjacency_prob();

    temporal_component<EdgeT, EstimatorT>
      in_component(seed, edge_size_est, node_size_est);
    in_component.insert(root, root.mutator_verts());

    std::unordered_map<VertexType, TimeType> last_infected;
    last_infected.reserve(eg.events_effect().size());

    for (auto && v: root.mutator_verts())
      last_infected[v] = root.cause_time();

    TimeType last_infect_time = root.cause_time();

    auto begin = eg.events_effect().rbegin();
    auto end = eg.events_effect().rend();

    auto topo_it = std::upper_bound(begin, end, root,
        [](const EdgeT& a, const EdgeT& b){
          return a.effect_time() > b.effect_time();
        });

    while (topo_it < end &&
        (topo_it->effect_time() < last_infect_time ||
         topo_it->effect_time() - last_infect_time  < prob.cutoff_dt())) {
      while (!in_transition.empty() &&
          in_transition.top().cause_time() > topo_it->effect_time()) {
        for (auto && v: in_transition.top().mutator_verts()) {
          last_infected[v] = in_transition.top().cause_time();
        }

        auto event = in_transition.top();
        in_component.insert(event, event.mutator_verts());
        in_transition.pop();
      }


      bool is_infecting = false;

      for (auto && v: topo_it->mutated_verts()) {
        auto last_infected_it = last_infected.find(v);
        if (last_infected_it != last_infected.end() &&
            topo_it->effect_time() < last_infected_it->second &&
            last_infected_it->second - topo_it->effect_time() <
                prob.cutoff_dt())
          is_infecting = true;
      }

      if (is_infecting) {
        if (topo_it->cause_time() == topo_it->effect_time()) {
          in_component.insert(*topo_it, topo_it->mutator_verts());
          for (auto && v: topo_it->mutator_verts())
            last_infected[v] = topo_it->effect_time();
        } else {
          in_transition.push(*topo_it);
        }
        last_infect_time =
          std::min(topo_it->cause_time(), last_infect_time);
      }

      topo_it++;
    }

    while (!in_transition.empty()) {
      auto event = in_transition.top();
      in_component.insert(event, event.mutator_verts());
      in_transition.pop();
    }

    return in_component;
  }


  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t seed,
      std::size_t node_size_est,
      std::size_t edge_size_est,
      bool revert_graph) {
    if constexpr (std::is_same_v<AdjacencyProbT,
                                    adjacency_prob::deterministic<EdgeT>>)
      if (revert_graph)
        return _deterministic_in_component<EdgeT, EstimatorT>(
            eg, root, seed, node_size_est, edge_size_est);
      else
        return _deterministic_out_component<EdgeT, EstimatorT>(
            eg, root, seed, node_size_est, edge_size_est);
    else
      return _generic_out_component<EdgeT, AdjacencyProbT, EstimatorT>(
          eg, root, seed, node_size_est, edge_size_est, revert_graph);
  }

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t seed,
      std::size_t node_size_hint,
      std::size_t edge_size_hint) {
    return _out_component<EdgeT, AdjacencyProbT, EstimatorT>(
        eg, root, seed,
        node_size_hint, edge_size_hint, false);
  }

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  in_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t seed,
      std::size_t node_size_hint,
      std::size_t edge_size_hint) {
    return _out_component<EdgeT, AdjacencyProbT, EstimatorT>(
        eg, root, seed,
        node_size_hint, edge_size_hint, true);
  }

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  std::vector<temporal_component<EdgeT, EstimatorT>>
  weakly_connected_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      std::size_t seed,
      bool singletons) {
    auto disj_set = ds::disjoint_set<std::size_t>(eg.events_cause().size());

    auto temp_edge_iter = eg.events_cause().begin();

    constexpr bool reducable = is_reducable_v<
      implicit_event_graph<EdgeT, AdjacencyProbT>>;

    while (temp_edge_iter < eg.events_cause().end()) {
      std::size_t temp_edge_idx = static_cast<std::size_t>(
          std::distance(eg.events_cause().begin(), temp_edge_iter));

      for (auto&& other:
            eg.successors(*temp_edge_iter, reducable)) {
        auto other_it = std::lower_bound(
            temp_edge_iter+1,
            eg.events_cause().end(),
            other);

        std::size_t other_idx = static_cast<std::size_t>(
            std::distance(eg.events_cause().begin(), other_it));
        disj_set.merge(temp_edge_idx, other_idx);
      }

      temp_edge_iter++;
    }

    auto sets = disj_set.sets(singletons);
    std::vector<temporal_component<EdgeT, EstimatorT>> sets_vector;
    sets_vector.reserve(sets.size());

    for (const auto& [idx, set]: sets) {
      sets_vector.emplace_back(seed, set.size(), set.size());
      auto& current_set = sets_vector.back();
      for (const auto& event_idx: set) {
        auto event = eg.events_cause().at(event_idx);
        std::unordered_set<typename EdgeT::VertexType> nodes;

        for (auto&& v: event.mutator_verts())
          nodes.insert(v);
        for (auto&& v: event.mutated_verts())
          nodes.insert(v);

        current_set.insert(event,
            std::vector<typename EdgeT::VertexType>(
              nodes.begin(), nodes.end()));
      }
    }

    return sets_vector;
  }
}  // namespace dag
