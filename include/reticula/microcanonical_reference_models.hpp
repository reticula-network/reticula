#ifndef INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_
#define INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  namespace microcanonical_reference_models {
    /**
      Produces a random shuffling of the temporal network where each event is
      attributed to two randomly selected vertices from the original network.
      Equivalent to micocanonical reference model  with the canonical name
      $P[E]$.

      The set of vertices, timestamps and the number of events are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> instant_event_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where all events
      between two vertices are attributed to two randomly selected vertecies
      from the original network. Equivalent to micocanonical reference model
      with the canonical name $P[p_{\mathcal{L}}(\Theta)]$.

      The set of vertices, timestamps, the number of events and the multiset
      of timelines are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> link_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where all events
      between two vertices are attributed to two randomly selected vertecies
      from the original network. As opposed to `link_shuffling`, this model
      preserves the pattern of (weak) connectivity in the static projection of
      the original graph, i.e., the static projection of the output would
      have the same set of (weakly) connected components as the input.
      Generalisation of the micocanonical reference model with the canonical
      name $P[I_\lambda, p_{\mathcal{L}}(\Theta)]$ to temporal networks with
      directed and/or multi-component stateic projections.

      In addition to the set of components of the static projection, the set of
      vertices, timestamps, the number of events and the multiset of timelines
      of the temporal network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> connected_link_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where the complete
      timelines of pairs of connected vertices are randomly swapped, wholly,
      with each other. Equivalent to micocanonical reference model
      with the canonical name $P[\mathcal{L}, p_{\mathcal{L}}(\Theta)]$.

      The set of vertices, timestamps, the set of timeline cardinalities and the
      static projection of the temporal network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> topology_constrained_link_shuffling(
        const network<EdgeT>& temp, Gen& generator);


    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timetamps and moving it to a
      randomly selected link with a non-empty timeline. Equivalent to
      micocanonical reference model with the canonical name $P[\mathcal{L}, E]$.

      The set of vertices, total number of events and the static projection of
      the temporal network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);


    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timetamps without changing the
      static projection link (the vertices) each event is attributed to.
      Equivalent to micocanonical reference model with the canonical name
      $P[\mathbf{w}]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is passed through parameters `t_start` and
      `t_end`.

      The set of vertices, cardinality of each timeline and the static
      projection of the temporal network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> weight_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator,
        typename EdgeT::TimeType t_start, typename EdgeT::TimeType t_end);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timetamps without changing the
      static projection link (the vertices) each event is attributed to.
      Equivalent to micocanonical reference model with the canonical name
      $P[\mathbf{w}]$.

      The observation window, the window where the original measurement of the
      temporal network was made, is derived by minimum and maximum cause time of
      the events.

      The set of vertices, cardinality of each timeline and the static
      projection of the temporal network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> weight_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by assigning new, uniformly random timetamps without changing the
      static projection link (the vertices) each event is attributed to. The new
      timestamps are selected uniformly at random from first cause time to the
      last cause time (inclusive) of each timeline. Equivalent to micocanonical
      reference model with the canonical name
      $P[\mathbf{w}, \mathbf{t}^1, \mathbf{t}^w]$.

      The set of vertices, cardinality of each timeline and the static
      projection of the temporal network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> activity_constrained_timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);

    /**
      Produces a random shuffling of the temporal network where the events are
      shuffled by shuffling the inter-event times between them, without changing
      the static projection link (the vertices) each event is attributed to or
      the ordering of events in each timeline. Equivalent to micocanonical
      reference model with the canonical name
      $P[\mathbf{\pi}_\mathcal{L}(\mathbf{\delta \tau}), \mathbf{t}^1]$.

      The set of vertices, cardinality of each timeline, the set of inter-event
      times in each timeline and the static projection of the temporal network
      are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> inter_event_shuffling(
        const network<EdgeT>& temp, Gen& generator);

  }  // namespace microcanonical_reference_models

  namespace mrrms = microcanonical_reference_models;
}  // namespace reticula

#include "../../src/microcanonical_reference_models.tpp"

#endif  // INCLUDE_RETICULA_MICROCANONICAL_REFERENCE_MODELS_HPP_
