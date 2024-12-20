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

#include "../../src/microcanonical_reference_models.tpp"

#endif  // INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_
