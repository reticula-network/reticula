#ifndef INCLUDE_DAG_MICROCANONICAL_REFERENCE_MODELS_HPP_
#define INCLUDE_DAG_MICROCANONICAL_REFERENCE_MODELS_HPP_

#include "network_concepts.hpp"

namespace dag {
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

      The set of vertices, timestamps and the static projection of the temporal
      network are conserved.
    */
    template <temporal_edge EdgeT, std::uniform_random_bit_generator Gen>
    requires is_dyadic_v<EdgeT>
    network<EdgeT> timeline_shuffling(
        const network<EdgeT>& temp, Gen& generator);

  }  // namespace microcanonical_reference_models

  namespace mrrms = microcanonical_reference_models;
}  // namespace dag

#include "../../src/microcanonical_reference_models.tpp"

#endif  // INCLUDE_DAG_MICROCANONICAL_REFERENCE_MODELS_HPP_
