#include <queue>
#include <vector>

#include <ds/disjoint_set.hpp>

namespace dag {
  namespace detail {
    template <typename Comp, typename AdjT, typename TempResType>
    struct ieg_component_type_constructor {
      Comp operator()(AdjT adj, TempResType dt,
          std::size_t size_est, std::size_t seed);
    };


    template <temporal_adjacency::temporal_adjacency AdjT>
    struct ieg_component_type_constructor<
        component<typename AdjT::EdgeType>,
        AdjT, typename AdjT::EdgeType::TimeType> {
      component<typename AdjT::EdgeType>
      operator()(AdjT /* adj */, typename AdjT::EdgeType::TimeType /* dt */,
          std::size_t size_est, std::size_t /* seed */) {
        return component<typename AdjT::EdgeType>(size_est);
      }
    };

    template <temporal_adjacency::temporal_adjacency AdjT>
    struct ieg_component_type_constructor<
        component_sketch<typename AdjT::EdgeType>,
        AdjT, typename AdjT::EdgeType::TimeType> {
      component_sketch<typename AdjT::EdgeType>
      operator()(AdjT /* adj */, typename AdjT::EdgeType::TimeType /* dt */,
          std::size_t /* size_est */, std::size_t seed) {
        return component_sketch<typename AdjT::EdgeType>(seed);
      }
    };

    template <
      temporal_edge EdgeT,
      temporal_adjacency::temporal_adjacency AdjT,
      typename IntermComponent,
      typename OutputComponent>
    std::vector<std::pair<EdgeT, OutputComponent>>
    out_components(
        const implicit_event_graph<EdgeT, AdjT>& eg,
        typename EdgeT::TimeType temporal_resolution,
        std::size_t seed) {
      std::unordered_map<EdgeT, IntermComponent, hash<EdgeT>>
        out_components;
      std::vector<std::pair<EdgeT, OutputComponent>>
        out_component_ests;
      out_component_ests.reserve(eg.events_cause().size());

      std::unordered_map<EdgeT, std::size_t, hash<EdgeT>> in_degrees;

      auto temp_edge_iter = eg.events_cause().rbegin();
      auto end = eg.events_cause().rend();

      while (temp_edge_iter < end) {
        out_components.emplace(*temp_edge_iter,
          ieg_component_type_constructor<
              IntermComponent, AdjT, typename AdjT::EdgeType::TimeType>{}(
            eg.temporal_adjacency(), temporal_resolution, 0, seed));

        std::vector<EdgeT> successors = eg.successors(
            *temp_edge_iter, true);
        std::vector<EdgeT> predecessors = eg.predecessors(
            *temp_edge_iter, true);

        in_degrees[*temp_edge_iter] = predecessors.size();

        for (const auto& other: successors) {
          out_components.at(*temp_edge_iter).merge(out_components.at(other));

          if (--in_degrees.at(other) == 0) {
            out_component_ests.emplace_back(other, out_components.at(other));
            out_components.erase(other);
            in_degrees.erase(other);
          }
        }

        out_components.at(*temp_edge_iter).insert(*temp_edge_iter);

        if (in_degrees.at(*temp_edge_iter) == 0) {
          out_component_ests.emplace_back(*temp_edge_iter,
            out_components.at(*temp_edge_iter));
          out_components.erase(*temp_edge_iter);
          in_degrees.erase(*temp_edge_iter);
        }
        temp_edge_iter++;
      }

      return out_component_ests;
    }

    template <
      temporal_edge EdgeT,
      temporal_adjacency::temporal_adjacency AdjT,
      typename IntermComponent,
      typename OutputComponent>
    std::vector<std::pair<EdgeT, OutputComponent>>
    in_components(
        const implicit_event_graph<EdgeT, AdjT>& eg,
        typename EdgeT::TimeType temporal_resolution,
        std::size_t seed) {
      std::unordered_map<EdgeT, IntermComponent, hash<EdgeT>>
        in_components;
      std::vector<std::pair<EdgeT, OutputComponent>>
        in_component_ests;
      in_component_ests.reserve(eg.events_cause().size());

      std::unordered_map<EdgeT, std::size_t, hash<EdgeT>> out_degrees;

      auto temp_edge_iter = eg.events_effect().begin();
      auto end = eg.events_effect().end();

      while (temp_edge_iter < end) {
        in_components.emplace(*temp_edge_iter,
          ieg_component_type_constructor<
              IntermComponent, AdjT, typename AdjT::EdgeType::TimeType>{}(
            eg.temporal_adjacency(), temporal_resolution, 0, seed));

        std::vector<EdgeT> successors = eg.successors(
            *temp_edge_iter, true);
        std::vector<EdgeT> predecessors = eg.predecessors(
            *temp_edge_iter, true);

        out_degrees[*temp_edge_iter] = successors.size();

        for (const auto& other: predecessors) {
          in_components.at(*temp_edge_iter).merge(in_components.at(other));

          if (--out_degrees.at(other) == 0) {
            in_component_ests.emplace_back(other, in_components.at(other));
            in_components.erase(other);
            out_degrees.erase(other);
          }
        }

        auto nodes = temp_edge_iter->mutator_verts();
        in_components.at(*temp_edge_iter).insert(*temp_edge_iter);

        if (out_degrees.at(*temp_edge_iter) == 0) {
          in_component_ests.emplace_back(*temp_edge_iter,
            in_components.at(*temp_edge_iter));
          in_components.erase(*temp_edge_iter);
          out_degrees.erase(*temp_edge_iter);
        }
        temp_edge_iter++;
      }

      return in_component_ests;
    }

    template <
      temporal_edge EdgeT,
      temporal_adjacency::temporal_adjacency AdjT>
    dag::component<EdgeT>
    generic_out_component(
        const implicit_event_graph<EdgeT, AdjT>& eg,
        const EdgeT& root,
        bool revert_graph,
        bool ignore_direction) {
      dag::component<EdgeT> out_component({root});
      std::queue<EdgeT> search({root});

      while (!search.empty()) {
        EdgeT e = search.front();
        search.pop();

        std::vector<EdgeT> next;
        if (ignore_direction)
          next = eg.neighbours(e, true);
        else if (revert_graph)
          next = eg.predecessors(e, true);
        else
          next = eg.successors(e, true);

        for (auto&& s: next)
          if (!out_component.contains(s)) {
            search.push(s);
            out_component.insert(s);
          }
      }

      return out_component;
    }
  }  // namespace detail

  // in-components
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component<EdgeT>>>
  in_components(
      const implicit_event_graph<EdgeT, AdjT>& eg) {
    return detail::in_components<
      EdgeT, AdjT, component<EdgeT>, component<EdgeT>>(eg, 0, 0);
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size<EdgeT>>>
  in_component_sizes(
      const implicit_event_graph<EdgeT, AdjT>& eg) {
    return detail::in_components<
      EdgeT, AdjT, component<EdgeT>, component_size<EdgeT>>(eg, 0, 0);
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size_estimate<EdgeT>>>
  in_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      std::size_t seed) {
    return detail::in_components<
      EdgeT, AdjT,
      component_sketch<EdgeT>,
      component_size_estimate<EdgeT>>(eg, 0, seed);
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  component<EdgeT>
  in_component(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      const EdgeT& root) {
    return detail::generic_out_component(eg, root, true, false);
  }


  // out-components
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component<EdgeT>>>
  out_components(
      const implicit_event_graph<EdgeT, AdjT>& eg) {
    return detail::out_components<
      EdgeT, AdjT, component<EdgeT>, component<EdgeT>>(eg, 0, 0);
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size<EdgeT>>>
  out_component_sizes(
      const implicit_event_graph<EdgeT, AdjT>& eg) {
    return detail::out_components<
      EdgeT, AdjT, component<EdgeT>, component_size<EdgeT>>(eg, 0, 0);
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size_estimate<EdgeT>>>
  out_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      std::size_t seed) {
    return detail::out_components<
      EdgeT, AdjT,
      component_sketch<EdgeT>,
      component_size_estimate<EdgeT>>(eg, seed);
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  component<EdgeT>
  out_component(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      const EdgeT& root) {
    return detail::generic_out_component(eg, root, false, false);
  }

  // weakly connected components
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<component<EdgeT>>
  weakly_connected_components(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      bool singletons) {
    auto disj_set = ds::disjoint_set<std::size_t>(eg.events_cause().size());

    auto temp_edge_iter = eg.events_cause().begin();

    while (temp_edge_iter < eg.events_cause().end()) {
      std::size_t temp_edge_idx = static_cast<std::size_t>(
          std::distance(eg.events_cause().begin(), temp_edge_iter));

      for (auto&& other:
            eg.successors(*temp_edge_iter, true)) {
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
    std::vector<component<EdgeT>> comp_vector;
    comp_vector.reserve(sets.size());

    for (const auto& [idx, set]: sets) {
      auto& current_set = comp_vector.emplace_back(set.size());
      for (const auto& event_idx: set)
        current_set.insert(eg.events_cause().at(event_idx));
    }

    return comp_vector;
  }

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  component<EdgeT>
  weakly_connected_component(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      const EdgeT& root) {
    return detail::generic_out_component(eg, root, false, true);
  }
}  // namespace dag
