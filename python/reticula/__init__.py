from ._reticula_ext import __version__

from ._reticula_ext import mersenne_twister

from ._reticula_ext import (
    undirected_edge, directed_edge,
    undirected_temporal_edge, directed_temporal_edge,
    directed_delayed_temporal_edge,
)

from ._reticula_ext import (
    undirected_network, directed_network,
    directed_temporal_network, undirected_temporal_network,
    directed_delayed_temporal_network,
    undirected_hypernetwork, directed_hypernetwork,
    directed_temporal_hypernetwork, undirected_temporal_hypernetwork,
    directed_delayed_temporal_hypernetwork,
)


# Operations

from ._reticula_ext import (
    with_vertices,
    with_edges,
    without_isolated_vertices,
    without_vertices,
    without_edges,
    uniformly_occupy_vertices,
    occupy_vertices,
    uniformly_occupy_edges,
    occupy_edges,
    static_projection,
    directed_projection,
    clique_expansion,
    vertex_induced_subgraph,
    edge_induced_subgraph,
    relabel,
)

# Generation

from ._reticula_generation import (
    path_graph, cycle_graph, star_graph, wheel_graph, complete_graph,
    complete_bipartite_graph, complete_bipartite_directed_graph,
    complete_directed_graph, regular_ring_lattice
)

from ._reticula_generation import (
    random_gnp_graph, random_directed_gnp_graph,
    random_gnm_graph, random_directed_gnm_graph,
    random_barabasi_albert_graph,
    random_watts_strogatz_graph
)

from ._reticula_generation import (
    try_random_regular_graph,
    random_regular_graph,
    try_random_directed_regular_graph,
    random_directed_regular_graph,
    expected_degree_sequence_graph,
    directed_expected_degree_sequence_graph,
    expected_degree_sequence_hypergraph,
    directed_expected_degree_sequence_hypergraph,
    degree_sequence_graph,
    try_degree_sequence_graph,
    directed_degree_sequence_graph,
    try_directed_degree_sequence_graph,
    random_uniform_hypergraph,
    random_directed_uniform_hypergraph,
    random_fully_mixed_temporal_network,
    random_directed_fully_mixed_temporal_network,
    random_link_activation_temporal_network,
    random_node_activation_temporal_network,
)

__all__ = [
    "__version__",
    "mersenne_twister",
    # edges
    "undirected_edge",
    "directed_edge",
    "undirected_temporal_edge",
    "directed_temporal_edge",
    "directed_delayed_temporal_edge",
    # networks
    "undirected_network",
    "directed_network",
    "undirected_temporal_network",
    "directed_temporal_network",
    "directed_delayed_temporal_network",
    "undirected_hypernetwork",
    "directed_hypernetwork",
    "undirected_temporal_hypernetwork",
    "directed_temporal_hypernetwork",
    "directed_delayed_temporal_hypernetwork",

    # Operations
    # - algebraic
    # - addition
    "with_vertices",
    "with_edges",
    # - removal
    "without_isolated_vertices",
    "without_vertices",
    "without_edges",
    # - occupation
    "uniformly_occupy_vertices",
    "occupy_vertices",
    "uniformly_occupy_edges",
    "occupy_edges",
    # - projections
    "static_projection",
    "directed_projection",
    "clique_expansion",
    # - subgraphs
    "vertex_induced_subgraph",
    "edge_induced_subgraph",
    # - relabel
    "relabel",

    # generation
    # - deterministic
    "path_graph",
    "cycle_graph",
    "star_graph",
    "wheel_graph",
    "complete_graph",
    "complete_bipartite_graph",
    "complete_bipartite_directed_graph",
    "complete_directed_graph",
    "regular_ring_lattice",
    # - gnp
    "random_gnp_graph",
    "random_directed_gnp_graph",
    "random_gnm_graph",
    "random_directed_gnm_graph",
    # - ba
    "random_barabasi_albert_graph",
    # - ws
    "random_watts_strogatz_graph",
    # - regular
    "try_random_regular_graph",
    "random_regular_graph",
    "try_random_directed_regular_graph",
    "random_directed_regular_graph",
    # - expected degree sequence
    "expected_degree_sequence_graph",
    "directed_expected_degree_sequence_graph",
    "expected_degree_sequence_hypergraph",
    "directed_expected_degree_sequence_hypergraph",
    # - degree sequence
    "degree_sequence_graph",
    "try_degree_sequence_graph",
    "directed_degree_sequence_graph",
    "try_directed_degree_sequence_graph",
    # - uniform hypergraph
    "random_uniform_hypergraph",
    "random_directed_uniform_hypergraph",
    # - fully mixed temporal network
    "random_fully_mixed_temporal_network",
    "random_directed_fully_mixed_temporal_network",
    # - link activation
    "random_link_activation_temporal_network",
    # - node activation
    "random_node_activation_temporal_network",

]
