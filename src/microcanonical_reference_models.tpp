#include <unordered_set>
#include <concepts>

#include "../include/dag/temporal_algorithms.hpp"

namespace dag {
  namespace microcanonical_reference_models {
    namespace detail {
      /* template <std::integral TimeT> */
      /* sample_times(TimeT start, TimeT end, std::size_t num) { */
      /*   std::vector<TimeT> */
      /* } */

      template <temporal_edge EdgeT>
      struct replace_verts {
        EdgeT operator()(
            const EdgeT& e,
            typename EdgeT::VertexType i,
            typename EdgeT::VertexType j);
      };

      template <network_vertex VertT, typename TimeT>
      struct replace_verts<undirected_temporal_edge<VertT, TimeT>> {
        undirected_temporal_edge<VertT, TimeT>
        operator()(
            const undirected_temporal_edge<VertT, TimeT>& e, VertT i, VertT j) {
          return {i, j, e.cause_time()};
        }
      };

      template <network_vertex VertT, typename TimeT>
      struct replace_verts<directed_temporal_edge<VertT, TimeT>> {
        directed_temporal_edge<VertT, TimeT>
        operator()(
            const directed_temporal_edge<VertT, TimeT>& e, VertT i, VertT j) {
          return {i, j, e.cause_time()};
        }
      };

      template <network_vertex VertT, typename TimeT>
      struct replace_verts<directed_delayed_temporal_edge<VertT, TimeT>> {
        directed_delayed_temporal_edge<VertT, TimeT>
        operator()(
            const directed_delayed_temporal_edge<VertT, TimeT>& e,
            VertT i, VertT j) {
          return {i, j, e.cause_time(), e.effect_time()};
        }
      };
    }  // namespace detail

    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> instant_event_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto nodes = temp.vertices();

      std::vector<EdgeT> shuffled;
      shuffled.reserve(temp.edges_cause().size());

      std::uniform_int_distribution<std::size_t> d(0, nodes.size() - 1);
      auto sample_verts = [&d](Gen& gen) {
        std::size_t i = d(gen), j = d(gen);
        while (i == j) j = d(gen);
        return std::make_pair(i, j);
      };

      std::unordered_set<EdgeT, hash<EdgeT>> current_set;
      current_set.reserve(32);

      auto last_cause_time = temp.edges_cause().front().cause_time();
      for (auto& event: temp.edges_cause()) {
        if (event.cause_time() > last_cause_time) {
          shuffled.insert(
              shuffled.end(), current_set.begin(), current_set.end());
          current_set.clear();
          last_cause_time = event.cause_time();
        }

        auto [i, j] = sample_verts(generator);
        auto new_event = detail::replace_verts<EdgeT>{}(
            event, nodes[i], nodes[j]);
        while (current_set.contains(new_event)) {
          std::tie(i, j) = sample_verts(generator);
          new_event = detail::replace_verts<EdgeT>{}(event, nodes[i], nodes[j]);
        }
        current_set.insert(new_event);
      }

      shuffled.insert(shuffled.end(), current_set.begin(), current_set.end());
      return network<EdgeT>(shuffled, nodes);
    }


    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> link_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto nodes = temp.vertices();

      std::uniform_int_distribution<std::size_t> d(0, nodes.size() - 1);
      auto proj = static_projection(temp);

      std::unordered_set<
        std::pair<
          typename EdgeT::VertexType,
          typename EdgeT::VertexType>,
        hash<
          std::pair<
            typename EdgeT::VertexType,
            typename EdgeT::VertexType>>> new_links;
      while (new_links.size() < proj.edges().size()) {
        std::size_t i = d(generator), j = d(generator);
        while (i == j) j = d(generator);
        new_links.emplace(nodes[i], nodes[j]);
      }

      std::vector<
        std::pair<
          typename EdgeT::VertexType,
          typename EdgeT::VertexType>> new_links_v(
              new_links.begin(), new_links.end());
      std::ranges::shuffle(new_links_v, generator);

      std::unordered_map<
        typename EdgeT::StaticProjectionType,
        std::pair<
          typename EdgeT::VertexType,
          typename EdgeT::VertexType>,
        hash<typename EdgeT::StaticProjectionType>> link_map;
      for (std::size_t i = 0; i < new_links_v.size(); i++)
        link_map.emplace(proj.edges()[i], new_links_v[i]);

      return network<EdgeT>(
          temp.edges_cause() | std::views::transform(
            [&link_map](const auto& e){
              auto& [ni, nj] = link_map.at(e.static_projection());
              return detail::replace_verts<EdgeT>{}(e, ni, nj);
            }), temp.vertices());
    }

    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> connected_link_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      auto proj = dag::static_projection(temp);
      auto proj_ccs = dag::weakly_connected_components(proj);
      std::ranges::sort(proj_ccs, std::ranges::greater{}, std::ranges::size);

      network<EdgeT> res(std::vector<EdgeT>(), proj.vertices());
      for (auto& comp: proj_ccs) {
        auto sub_temp = dag::vertex_induced_subgraph(temp, comp);
        network<EdgeT> shuff(std::vector<EdgeT>(), sub_temp.vertices());
        while (!is_weakly_connected(static_projection(shuff)))
          shuff = link_shuffling(sub_temp, generator);
        res = dag::graph_union(res, shuff);
      }

      return res;
    }

    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto proj = static_projection(temp);
      auto shuffled_links = proj.edges();
      std::ranges::shuffle(shuffled_links, generator);

      std::vector<EdgeT> shuffled_edges;
      shuffled_edges.reserve(temp.edges_cause().size());

      std::unordered_map<
        typename EdgeT::StaticProjectionType,
        std::pair<
          typename EdgeT::VertexType,
          typename EdgeT::VertexType>,
        hash<typename EdgeT::StaticProjectionType>> link_map;

      for (auto& e: proj.edges()) {
        auto ni = e.incident_verts().front();
        auto nj = e.incident_verts().back();
        if constexpr (!is_undirected_v<typename EdgeT::StaticProjectionType>) {
          ni = e.tail();
          nj = e.head();
        }

        link_map.emplace(e, std::make_pair(ni, nj));
      }

      return network<EdgeT>(
          temp.edges_cause() | std::views::transform(
            [&link_map](const auto& e){
              auto& [ni, nj] = link_map.at(e.static_projection());
              return detail::replace_verts<EdgeT>{}(e, ni, nj);
            }), temp.vertices());
    }
  }  // namespace microcanonical_reference_models
}  // namespace dag
