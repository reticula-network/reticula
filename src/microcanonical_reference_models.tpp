#include <unordered_set>
#include <unordered_map>
#include <concepts>

#include "../include/reticula/temporal_algorithms.hpp"
#include "../include/reticula/components.hpp"
#include "../include/reticula/operations.hpp"

namespace reticula {
  namespace microcanonical_reference_models {
    namespace detail {
      template <
        std::floating_point TimeT,
        std::uniform_random_bit_generator Gen>
      std::vector<TimeT>
      sample_timestamps(TimeT t0, TimeT t1, std::size_t k, Gen& generator) {
        std::unordered_set<TimeT> s;
        s.reserve(k);

        // uniform_real_distribution is not end-inclusive.
        std::uniform_real_distribution<TimeT> dist(
            t0, std::nextafter(t1, std::numeric_limits<TimeT>::infinity()));
        while (s.size() < k)
          s.insert(dist(generator));

        return std::vector<TimeT>(s.begin(), s.end());
      }

      template <
        std::integral TimeT,
        std::uniform_random_bit_generator Gen>
      std::vector<TimeT>
      sample_timestamps(TimeT t0, TimeT t1, std::size_t k, Gen& generator) {
        std::vector<TimeT> out;
        out.reserve(static_cast<TimeT>(k));
        for (TimeT i = t0; i < t0 + static_cast<TimeT>(k); i++)
          out.push_back(i);

        double w = std::exp(
            std::log(
              std::uniform_real_distribution{}(
                generator))/static_cast<double>(k));

        TimeT i = t0 + static_cast<TimeT>(k) + static_cast<TimeT>(
              std::log(
                std::uniform_real_distribution{}(
                  generator))/std::log(1-w)) + 1;

        while (i <= t1) {
          std::uniform_int_distribution<TimeT> dist{0, static_cast<TimeT>(k)-1};
          out[dist(generator)] = i;
          i += static_cast<TimeT>(
              std::log(
                std::uniform_real_distribution{}(
                  generator))/std::log(1-w)) + 1;
        }

        return out;
      }

      template <temporal_network_edge EdgeT>
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

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
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


    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
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
      ranges::shuffle(new_links_v, generator);

      std::unordered_map<
        typename EdgeT::StaticProjectionType,
        std::pair<
          typename EdgeT::VertexType,
          typename EdgeT::VertexType>,
        hash<typename EdgeT::StaticProjectionType>> link_map;
      for (std::size_t i = 0; i < new_links_v.size(); i++)
        link_map.emplace(proj.edges()[i], new_links_v[i]);

      return network<EdgeT>(
          temp.edges_cause() | views::transform(
            [&link_map](const auto& e){
              auto& [ni, nj] = link_map.at(e.static_projection());
              return detail::replace_verts<EdgeT>{}(e, ni, nj);
            }), temp.vertices());
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> connected_link_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      auto proj = static_projection(temp);

      std::vector<component<typename EdgeT::VertexType>> proj_ccs;
      if constexpr (is_undirected_v<typename EdgeT::StaticProjectionType>)
        proj_ccs = connected_components(proj);
      else
        proj_ccs = weakly_connected_components(proj);

      ranges::sort(proj_ccs, ranges::greater{}, ranges::size);

      auto is_projection_connected = [](const network<EdgeT>& shuff) {
        if constexpr (is_undirected_v<typename EdgeT::StaticProjectionType>)
          return is_connected(static_projection(shuff));
        else
          return is_weakly_connected(static_projection(shuff));
      };

      network<EdgeT> res(std::vector<EdgeT>(), proj.vertices());
      for (auto& comp: proj_ccs) {
        auto sub_temp = vertex_induced_subgraph(temp, comp);
        network<EdgeT> shuff(std::vector<EdgeT>(), sub_temp.vertices());
        while (!is_projection_connected(shuff))
          shuff = link_shuffling(sub_temp, generator);
        res = graph_union(res, shuff);
      }

      return res;
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto proj = static_projection(temp);
      auto shuffled_links = proj.edges();
      ranges::shuffle(shuffled_links, generator);

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
          temp.edges_cause() | views::transform(
            [&link_map](const auto& e){
              auto& [ni, nj] = link_map.at(e.static_projection());
              return detail::replace_verts<EdgeT>{}(e, ni, nj);
            }), temp.vertices());
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end) {
      auto [t0, t1] = cause_time_window(temp);
      if (t0 < t_start || t1 > t_end)
        throw std::invalid_argument("observation window does not cover "
            "the cause time of all the events");

      std::vector<EdgeT> shuffled_edges;
      shuffled_edges.reserve(temp.edges_cause().size());

      auto p = static_projection(temp);
      auto& links = p.edges();

      std::unordered_map<
        typename EdgeT::StaticProjectionType, std::vector<EdgeT>,
        hash<typename EdgeT::StaticProjectionType>> shuffled_link;
      std::uniform_int_distribution<std::size_t> dist(0, links.size() - 1);
      for (auto& l: links)
        shuffled_link[l].reserve(temp.edges().size()*2/links.size());

      for (auto& e: temp.edges_cause())
        shuffled_link[links[dist(generator)]].push_back(e);

      for (auto& [l, tl]: shuffled_link) {
        auto ts = detail::sample_timestamps(t_start, t_end,
            tl.size(), generator);
        for (std::size_t i = 0; i < tl.size(); i++) {
          auto e = tl[i];
          if constexpr (std::same_as<
            EdgeT, directed_delayed_temporal_edge<
              typename EdgeT::VertexType, typename EdgeT::TimeType>>)
            shuffled_edges.emplace_back(l,
                ts[i], ts[i] + e.effect_time() - e.cause_time());
          else
            shuffled_edges.emplace_back(l, ts[i]);
        }
      }

      return network<EdgeT>(shuffled_edges, temp.vertices());
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      auto [t0, t1] = cause_time_window(temp);
      return timeline_shuffling(temp, generator, t0, t1);
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> weight_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto [t0, t1] = cause_time_window(temp);
      if (t0 < t_start || t1 > t_end)
        throw std::invalid_argument("observation window does not cover "
            "the cause time of all the events");

      std::vector<EdgeT> shuffled_edges;
      shuffled_edges.reserve(temp.edges_cause().size());

      for (auto tls = link_timelines(temp); auto& [link, timeline]: tls) {
        auto ts = detail::sample_timestamps(t_start, t_end,
            timeline.size(), generator);
        for (std::size_t i = 0; i < timeline.size(); i++) {
          auto e = timeline[i];
          if constexpr (std::same_as<
            EdgeT, directed_delayed_temporal_edge<
              typename EdgeT::VertexType, typename EdgeT::TimeType>>)
            shuffled_edges.emplace_back(e.static_projection(),
                ts[i], ts[i] + e.effect_time() - e.cause_time());
          else
            shuffled_edges.emplace_back(e.static_projection(), ts[i]);
        }
      }

      return network<EdgeT>(shuffled_edges, temp.vertices());
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> weight_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      auto [t0, t1] = cause_time_window(temp);
      return weight_constrained_timeline_shuffling(temp, generator, t0, t1);
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> activity_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      std::vector<EdgeT> shuffled_edges;
      shuffled_edges.reserve(temp.edges_cause().size());

      for (auto tls = link_timelines(temp); auto& [link, timeline]: tls) {
        auto t_start = timeline.front().cause_time(),
             t_end = timeline.back().cause_time();
        auto ts = detail::sample_timestamps(t_start, t_end,
            timeline.size(), generator);
        for (std::size_t i = 0; i < timeline.size(); i++) {
          auto e = timeline[i];
          if constexpr (std::same_as<
            EdgeT, directed_delayed_temporal_edge<
              typename EdgeT::VertexType, typename EdgeT::TimeType>>)
            shuffled_edges.emplace_back(e.static_projection(),
                ts[i], ts[i] + e.effect_time() - e.cause_time());
          else
            shuffled_edges.emplace_back(e.static_projection(), ts[i]);
        }
      }

      return network<EdgeT>(shuffled_edges, temp.vertices());
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> inter_event_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      std::vector<EdgeT> shuffled_edges;
      shuffled_edges.reserve(temp.edges_cause().size());

      for (auto tls = link_timelines(temp); auto& [link, timeline]: tls) {
        std::vector<typename EdgeT::TimeType> iets;
        iets.reserve(timeline.size());

        for (std::size_t i = 1; i < timeline.size(); i++)
          iets.push_back(timeline[i].cause_time() - timeline[i-1].cause_time());

        ranges::shuffle(iets, generator);
        std::partial_sum(iets.begin(), iets.end(), iets.begin());

        auto t_start = timeline.front().cause_time();
        shuffled_edges.push_back(timeline.front());

        for (std::size_t i = 1; i < timeline.size(); i++) {
          auto e = timeline[i];
          if constexpr (std::same_as<
            EdgeT, directed_delayed_temporal_edge<
              typename EdgeT::VertexType, typename EdgeT::TimeType>>)
            shuffled_edges.emplace_back(
                e.static_projection(),
                iets[i-1] + t_start,
                iets[i-1] + t_start + e.effect_time() - e.cause_time());
          else
            shuffled_edges.emplace_back(
                e.static_projection(),
                iets[i-1] + t_start);
        }
      }

      return network<EdgeT>(shuffled_edges, temp.vertices());
    }
  }  // namespace microcanonical_reference_models
}  // namespace reticula
