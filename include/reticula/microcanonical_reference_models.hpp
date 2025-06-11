#ifndef INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_
#define INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  namespace microcanonical_reference_models {
    /**
      Produces a random shuffling of the temporal network where each event is
      attributed to two randomly selected vertices from the original network.
      Equivalent to microcanonical reference model with the canonical name
      $P[E]$.

      The set of vertices, timestamps and the number of events are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> instant_event_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where all events
      between two vertices are attributed to two randomly selected vertices
      from the original network. Equivalent to microcanonical reference model
      with the canonical name $P[p_{\mathcal{L}}(\Theta)]$.

      The set of vertices, timestamps, the number of events and the multiset
      of timelines are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> link_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where all events
      between two vertices are attributed to two randomly selected vertices
      from the original network. As opposed to `link_shuffling`, this model
      preserves the pattern of (weak) connectivity in the static projection of
      the original graph, i.e., the static projection of the output would
      have the same set of (weakly) connected components as the input.
      Generalisation of the microcanonical reference model with the canonical
      name $P[I_\lambda, p_{\mathcal{L}}(\Theta)]$ to temporal networks with
      directed and/or multi-component static projections.

      The observed static projection links in the network are complemented by
      the set of links passed through the parameter `unobserved_links`. This is
      used in case links are omitted in the static projection of the observed
      temporal network that are known to exist from other sources. Essentially,
      it is assumed that the links in `unobserved_links` would be present in the
      static projection of the temporal network had we had a long enough
      observation window.

      In addition to the set of components of the static projection, the set of
      vertices, timestamps, the number of events and the multiset of timelines
      of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> connected_link_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links);

    /**
      Produces a random shuffling of the temporal network where all events
      between two vertices are attributed to two randomly selected vertices
      from the original network. As opposed to `link_shuffling`, this model
      preserves the pattern of (weak) connectivity in the static projection of
      the original graph, i.e., the static projection of the output would
      have the same set of (weakly) connected components as the input.
      Generalisation of the microcanonical reference model with the canonical
      name $P[I_\lambda, p_{\mathcal{L}}(\Theta)]$ to temporal networks with
      directed and/or multi-component static projections.

      In addition to the set of components of the static projection, the set of
      vertices, timestamps, the number of events and the multiset of timelines
      of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> connected_link_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where the complete
      timelines of pairs of connected vertices are randomly swapped, wholly,
      with each other. Equivalent to microcanonical reference model
      with the canonical name $P[\mathcal{L}, p_{\mathcal{L}}(\Theta)]$.

      The observed static projection links in the network are complemented by
      the set of links passed through the parameter `unobserved_links`. This is
      used in case links are omitted in the static projection of the observed
      temporal network that are known to exist from other sources. Essentially,
      it is assumed that the links in `unobserved_links` would be present in the
      static projection of the temporal network had we had a long enough
      observation window.

      The set of vertices, timestamps, the set of timeline cardinalities and the
      static projection of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links);

    /**
      Produces a random shuffling of the temporal network where the complete
      timelines of pairs of connected vertices are randomly swapped, wholly,
      with each other. Equivalent to microcanonical reference model
      with the canonical name $P[\mathcal{L}, p_{\mathcal{L}}(\Theta)]$.

      The set of vertices, timestamps, the set of timeline cardinalities and the
      static projection of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator);


    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps and moving it to a
      randomly selected link with a non-empty timeline. Equivalent to
      microcanonical reference model with the canonical name $P[\mathcal{L}, E]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is passed through parameters `t_start` and
      `t_end`.

      The observed static projection links in the network are complemented by
      the set of links passed through the parameter `unobserved_links`. This is
      used in case links are omitted in the static projection of the observed
      temporal network that are known to exist from other sources. Essentially,
      it is assumed that the links in `unobserved_links` would be present in the
      static projection of the temporal network had we had a long enough
      observation window.

      The set of vertices, total number of events and the static projection of
      the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps and moving it to a
      randomly selected link with a non-empty timeline. Equivalent to
      microcanonical reference model with the canonical name $P[\mathcal{L}, E]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is passed through parameters `t_start` and
      `t_end`.

      The set of vertices, total number of events and the static projection of
      the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end);



    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps and moving it to a
      randomly selected link with a non-empty timeline. Equivalent to
      microcanonical reference model with the canonical name $P[\mathcal{L}, E]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is derived by minimum and maximum cause time of
      the events.

      The observed static projection links in the network are complemented by
      the set of links passed through the parameter `unobserved_links`. This is
      used in case links are omitted in the static projection of the observed
      temporal network that are known to exist from other sources. Essentially,
      it is assumed that the links in `unobserved_links` would be present in the
      static projection of the temporal network had we had a long enough
      observation window.

      The set of vertices, total number of events and the static projection of
      the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps and moving it to a
      randomly selected link with a non-empty timeline. Equivalent to
      microcanonical reference model with the canonical name $P[\mathcal{L}, E]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is derived by minimum and maximum cause time of
      the events.

      The set of vertices, total number of events and the static projection of
      the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);


    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps without changing the
      static projection link (the vertices) each event is attributed to.
      Equivalent to microcanonical reference model with the canonical name
      $P[\mathbf{w}]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is passed through parameters `t_start` and
      `t_end`.

      The set of vertices, cardinality of each timeline and the static
      projection of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> weight_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps without changing the
      static projection link (the vertices) each event is attributed to.
      Equivalent to microcanonical reference model with the canonical name
      $P[\mathbf{w}]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is derived by minimum and maximum cause time of
      the events.

      The set of vertices, cardinality of each timeline and the static
      projection of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> weight_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timestamps without changing the
      static projection link (the vertices) each event is attributed to. The new
      timestamps are selected uniformly at random from first cause time to the
      last cause time (inclusive) of each timeline. Equivalent to microcanonical
      reference model with the canonical name
      $P[\mathbf{w}, \mathbf{t}^1, \mathbf{t}^w]$.

      The set of vertices, cardinality of each timeline and the static
      projection of the temporal network are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> activity_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by shuffling the inter-event times between them, without changing
      the static projection link (the vertices) each event is attributed to or
      the ordering of events in each timeline. Equivalent to microcanonical
      reference model with the canonical name
      $P[\mathbf{\pi}_\mathcal{L}(\mathbf{\delta \tau}), \mathbf{t}^1]$.

      The set of vertices, cardinality of each timeline, the set of inter-event
      times in each timeline and the static projection of the temporal network
      are conserved.
    */
    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> inter_event_shuffling(
        const network<EdgeT>& temp, Gen& generator);


    /**
      Produces a random shuffling of the static network where the degree
      sequence (distribution of degrees) of the resulting network is the same
      as the original network.

      Equivalent to sampling from 1k-random graphs of the input graph, or
      canonical name $P[p(k)]$.

      The set of vertices, number of edges and degree distributions are
      preserved.
    */
    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> degree_sequence_preserving_shuffling(
        const network<EdgeT>& net,
        Gen& generator,
        std::size_t rewirings);

    /**
      Same as above but with $100 |E|$ rewirings.
     */
    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> degree_sequence_preserving_shuffling(
        const network<EdgeT>& net,
        Gen& generator);

    /**
      Produces a random shuffling of the static network where the joint-degree
      sequence (distribution of degrees on the two ends of each edge) of the
      resulting network is the same as the original network.

      Equivalent to sampling from 2k-random graphs of the input graph, or
      canonical name $P[p(k, k')]$.

      The set of vertices, number of edges, degree distribution and
      degree-degree distribution are preserved.
    */
    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> joint_degree_sequence_preserving_shuffling(
        const network<EdgeT>& net,
        Gen& generator,
        std::size_t rewirings);

    /**
      Same as above but with $100 |E|$ rewirings.
    */
    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> joint_degree_sequence_preserving_shuffling(
        const network<EdgeT>& net,
        Gen& generator);
  }  // namespace microcanonical_reference_models

  namespace mrrms = microcanonical_reference_models;
}  // namespace reticula

// Implementation
#include <random>
#include <ranges>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <concepts>

#include <indexed_set/indexed_set.hpp>

#include "network_concepts.hpp"
#include "components.hpp"
#include "temporal_algorithms.hpp"
#include "algorithms.hpp"
#include "operations.hpp"

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

      template <
        static_network_edge EdgeT,
        std::uniform_random_bit_generator Gen>
      requires is_dyadic_v<EdgeT>
      std::unordered_map<
        EdgeT,
        std::pair<
            typename EdgeT::VertexType,
            typename EdgeT::VertexType>,
        hash<EdgeT>>
      link_shuffling_mapping(const network<EdgeT>& proj, Gen& generator) {
        std::unordered_set<
          std::pair<
            typename EdgeT::VertexType,
            typename EdgeT::VertexType>,
          hash<
            std::pair<
              typename EdgeT::VertexType,
              typename EdgeT::VertexType>>> new_links;

        auto nodes = proj.vertices();
        std::uniform_int_distribution<std::size_t> d(0, nodes.size() - 1);
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
          EdgeT,
          std::pair<
            typename EdgeT::VertexType,
            typename EdgeT::VertexType>,
          hash<EdgeT>> link_map;
        for (std::size_t i = 0; i < new_links_v.size(); i++)
          link_map.emplace(proj.edges()[i], new_links_v[i]);
        return link_map;
      }
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
      auto proj = static_projection(temp);
      auto link_map = detail::link_shuffling_mapping(proj, generator);

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
      return connected_link_shuffling(temp, generator, {});
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> connected_link_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links) {
      auto proj = with_edges(static_projection(temp), unobserved_links);

      std::vector<component<typename EdgeT::VertexType>> proj_ccs;
      if constexpr (is_undirected_v<typename EdgeT::StaticProjectionType>)
        proj_ccs = connected_components(proj);
      else
        proj_ccs = weakly_connected_components(proj);

      ranges::sort(proj_ccs, ranges::greater{}, ranges::size);

      auto is_mapping_connected = [](
          const std::unordered_map<
            typename EdgeT::StaticProjectionType,
            std::pair<
              typename EdgeT::VertexType, typename EdgeT::VertexType>,
            hash<typename EdgeT::StaticProjectionType>> mapping) {
        auto edges = std::vector<typename EdgeT::StaticProjectionType>();
        for (auto& [k, v]: mapping)
          edges.emplace_back(v.first, v.second);
        network<typename EdgeT::StaticProjectionType> shuff(edges);
        if constexpr (is_undirected_v<typename EdgeT::StaticProjectionType>)
          return is_connected(shuff);
        else
          return is_weakly_connected(shuff);
      };

      network<EdgeT> res(std::vector<EdgeT>(), proj.vertices());
      for (auto& comp: proj_ccs) {
        if (comp.size() == 1) {
          res = with_vertices(res, comp);
        } else {
          auto sub_temp = vertex_induced_subgraph(temp, comp);
          auto proj_subgraph = vertex_induced_subgraph(proj, comp);
          auto mapping = detail::link_shuffling_mapping(
            proj_subgraph, generator);
          while (!is_mapping_connected(mapping))
            mapping = detail::link_shuffling_mapping(
              proj_subgraph, generator);
          network<EdgeT> shuff(
              sub_temp.edges_cause() | views::transform(
                [&mapping](const auto& e){
                  auto& [ni, nj] = mapping.at(e.static_projection());
                  return detail::replace_verts<EdgeT>{}(e, ni, nj);
                }), temp.vertices());
          res = graph_union(res, shuff);
        }
      }

      return res;
    }


    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator) {
      return topology_constrained_link_shuffling(temp, generator, {});
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto proj = with_edges(static_projection(temp), unobserved_links);
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
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto [t0, t1] = cause_time_window(temp);
      if (t0 < t_start || t1 > t_end)
        throw std::invalid_argument("observation window does not cover "
            "the cause time of all the events");

      std::vector<EdgeT> shuffled_edges;
      shuffled_edges.reserve(temp.edges_cause().size());

      auto p = with_edges(static_projection(temp), unobserved_links);
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
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto [t0, t1] = cause_time_window(temp);
      return timeline_shuffling(temp, generator, t0, t1, {});
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        const std::vector<
          typename EdgeT::StaticProjectionType>& unobserved_links) {
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

      auto [t0, t1] = cause_time_window(temp);
      return timeline_shuffling(temp, generator, t0, t1, unobserved_links);
    }

    template <
      temporal_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end) {
      return timeline_shuffling(temp, generator, t_start, t_end, {});
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
      if (temp.vertices().empty() || temp.edges().empty())
        return temp;

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


    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> degree_sequence_preserving_shuffling(
        const network<EdgeT>& net, Gen& generator, std::size_t rewirings) {
      indexed_set::indexed_set<EdgeT, hash<EdgeT>> edges;
      for (auto&& e: net.edges())
        edges.insert(e);

      auto verts = net.vertices();

      std::size_t rewires = 0;
      while (rewires < rewirings) {
        auto i = indexed_set::random_sample(edges, generator);
        auto inc_i = i.incident_verts();
        if (inc_i.size() != 2)
          throw std::invalid_argument("network cannot contain self-loops");

        auto r = inc_i[0];
        auto s = inc_i[1];
        std::size_t tries = 0;
        bool rewired = false;
        while (!rewired && tries < edges.size()) {
          tries++;
          auto j = indexed_set::random_sample(edges, generator);
          auto inc_j = j.incident_verts();
          if (inc_j.size() != 2)
            throw std::invalid_argument("network cannot contain self-loops");

          auto u = inc_j[0];
          auto v = inc_j[1];
          if (i == j || r == u || r == v || s == u || s == v)
            continue;

          EdgeT e1(r, v), e2(u, s);
          if (edges.contains(e1) || edges.contains(e2))
            continue;

          edges.erase(i);
          edges.erase(j);

          edges.insert(e1);
          edges.insert(e2);

          rewires++;
          rewired = true;
        }
      }

      return network<EdgeT>(edges, verts);
    }

    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> degree_sequence_preserving_shuffling(
        const network<EdgeT>& net, Gen& generator) {
      return degree_sequence_preserving_shuffling(
        net, generator, 100*net.edges().size());
    }


    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> joint_degree_sequence_preserving_shuffling(
        const network<EdgeT>& net, Gen& generator, std::size_t rewirings) {
      std::unordered_map<
        std::size_t,
        indexed_set::indexed_set<EdgeT, hash<EdgeT>>> edges_with_node_degree;
      for (auto&& e: net.edges()) {
        auto inc = e.incident_verts();
        if (inc.size() != 2)
          throw std::invalid_argument("network cannot contain self-loops");
        edges_with_node_degree[degree(net, inc[0])].insert(e);
        edges_with_node_degree[degree(net, inc[1])].insert(e);
      }

      auto verts = net.vertices();
      indexed_set::indexed_set<EdgeT, hash<EdgeT>> edges;
      for (auto&& e: net.edges())
        edges.insert(e);

      std::size_t rewires = 0;
      while (rewires < rewirings) {
        auto i = indexed_set::random_sample(edges, generator);
        auto inc_i = i.incident_verts();
        if (inc_i.size() != 2)
          throw std::invalid_argument("network cannot contain self-loops");

        auto r = inc_i[0];
        auto s = inc_i[1];
        auto k_r = degree(net, r);
        auto k_s = degree(net, s);
        if (std::bernoulli_distribution{}(generator)) {
          std::swap(r, s);
          std::swap(k_r, k_s);
        }

        if (edges_with_node_degree[k_r].size() < 2)
          continue;

        std::size_t tries = 0;
        bool rewired = false;
        while (!rewired && tries < edges.size()) {
          tries++;
          auto j = indexed_set::random_sample(
            edges_with_node_degree[k_r], generator);
          auto inc_j = j.incident_verts();
          if (inc_j.size() != 2)
            throw std::invalid_argument("network cannot contain self-loops");

          auto u = inc_j[0];
          auto v = inc_j[1];
          if (i == j || r == u || r == v || s == u || s == v)
            continue;

          auto k_u = degree(net, u);
          auto k_v = degree(net, v);
          if (k_u != k_r && k_v == k_r) {
            std::swap(u, v);
            std::swap(k_u, k_v);
          } else if ((k_u == k_r && k_v == k_r) &&
                std::bernoulli_distribution{}(generator)) {
              std::swap(u, v);
              std::swap(k_u, k_v);
          }

          EdgeT e1(r, v), e2(u, s);
          if (edges.contains(e1) || edges.contains(e2))
            continue;

          edges.erase(i);
          edges_with_node_degree[k_r].erase(i);
          edges_with_node_degree[k_s].erase(i);
          edges.erase(j);
          edges_with_node_degree[k_u].erase(j);
          edges_with_node_degree[k_v].erase(j);

          edges.insert(e1);
          edges_with_node_degree[k_r].insert(e1);
          edges_with_node_degree[k_v].insert(e1);
          edges.insert(e2);
          edges_with_node_degree[k_u].insert(e2);
          edges_with_node_degree[k_s].insert(e2);

          rewires++;
          rewired = true;
        }
      }

      return network<EdgeT>(edges, net.vertices());
    }

    template <
      undirected_static_network_edge EdgeT,
      std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> joint_degree_sequence_preserving_shuffling(
        const network<EdgeT>& net, Gen& generator) {
      return joint_degree_sequence_preserving_shuffling(
        net, generator, 100*net.edges().size());
    }
  }  // namespace microcanonical_reference_models
}  // namespace reticula


#endif  // INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_
