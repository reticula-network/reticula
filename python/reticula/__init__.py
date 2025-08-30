from ._reticula_ext import __version__

from ._reticula_ext import mersenne_twister

from ._reticula_ext import (
    undirected_edge, directed_edge,
    undirected_temporal_edge, directed_temporal_edge,
    directed_delayed_temporal_edge,
    undirected_hyperedge, directed_hyperedge,
    undirected_temporal_hyperedge, directed_temporal_hyperedge,
    directed_delayed_temporal_hyperedge,
)

from ._reticula_ext import (
    undirected_network, directed_network,
    directed_temporal_network, undirected_temporal_network,
    directed_delayed_temporal_network,
    undirected_hypernetwork, directed_hypernetwork,
    directed_temporal_hypernetwork, undirected_temporal_hypernetwork,
    directed_delayed_temporal_hypernetwork,
)

from ._reticula_ext import (
    vector_undirected_edge,
    vector_directed_edge,
    vector_undirected_temporal_edge,
    vector_directed_temporal_edge,
    vector_directed_delayed_temporal_edge,
    vector_undirected_hyperedge,
    vector_directed_hyperedge,
    vector_undirected_temporal_hyperedge,
    vector_directed_temporal_hyperedge,
    vector_directed_delayed_temporal_hyperedge,
    vector_uint64,
    vector_int64,
    vector_uint32,
    vector_int32,
    vector_double,
    vector_pair_uint64_uint64,
    vector_pair_double_double,
)

from ._reticula_ext import (
    span_undirected_edge,
    span_directed_edge,
    span_undirected_temporal_edge,
    span_directed_temporal_edge,
    span_directed_delayed_temporal_edge,
    span_undirected_hyperedge,
    span_directed_hyperedge,
    span_undirected_temporal_hyperedge,
    span_directed_temporal_hyperedge,
    span_directed_delayed_temporal_hyperedge,
    span_uint64,
    span_int64,
    span_uint32,
    span_int32,
    span_double,
    span_pair_uint64_uint64,
    span_pair_double_double,
)


from ._reticula_ext import (
    component)

# Operations
from ._reticula_operations import (
    graph_union,
    disjoint_union,
    graph_intersection,
    complement_graph,
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
    complete_directed_graph, regular_ring_lattice, square_grid_graph
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

from ._reticula_algorithms import (
    in_degree, out_degree,
    incident_degree, degree,
    in_degree_sequence, out_degree_sequence,
    incident_degree_sequence, degree_sequence,
    in_out_degree_pair_sequence,
    edge_in_degree,
    edge_out_degree,
    edge_incident_degree,
    edge_degree,
    edge_in_degree_sequence,
    edge_out_degree_sequence,
    edge_incident_degree_sequence,
    edge_degree_sequence,
    edge_in_out_degree_pair_sequence,
    density,
    time_window,
    cause_time_window,
    effect_time_window,
    link_timeline,
    window,
)

from ._reticula_algorithms import (
    weakly_connected_components,
    largest_weakly_connected_component,
    weakly_connected_component,
    is_weakly_connected,
    is_weakly_reachable,
    in_components,
    out_components,
    in_component,
    out_component,
    in_component_sizes,
    out_component_sizes,
    in_component_size_estimates,
    out_component_size_estimates,
    strongly_connected_components,
    largest_strongly_connected_component,
    strongly_connected_component,
    is_strongly_connected,
    is_strongly_reachable,
    connected_components,
    largest_connected_component,
    connected_component,
    is_connected,
    is_reachable,
    is_graphic,
    is_digraphic,
    is_acyclic,
    topological_order,
    try_topological_order,
    is_bipartite,
    try_two_colouring,
    two_colouring,
    attribute_assortativity,
    degree_assortativity,
    direction,
    shortest_path_lengths_from,
    shortest_path_lengths_to,
    try_diameter,
    diameter,
    try_out_eccentricity,
    out_eccentricity,
    try_in_eccentricity,
    in_eccentricity,
    try_out_radius,
    out_radius,
    try_in_radius,
    in_radius,
    try_eccentricity,
    eccentricity,
    try_radius,
    radius,
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
    "undirected_hyperedge",
    "directed_hyperedge",
    "undirected_temporal_hyperedge",
    "directed_temporal_hyperedge",
    "directed_delayed_temporal_hyperedge",
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
    # component
    "component",

    # Vectors
    "vector_undirected_edge",
    "vector_directed_edge",
    "vector_undirected_temporal_edge",
    "vector_directed_temporal_edge",
    "vector_directed_delayed_temporal_edge",
    "vector_undirected_hyperedge",
    "vector_directed_hyperedge",
    "vector_undirected_temporal_hyperedge",
    "vector_directed_temporal_hyperedge",
    "vector_directed_delayed_temporal_hyperedge",
    "vector_uint64",
    "vector_int64",
    "vector_uint32",
    "vector_int32",
    "vector_double",
    "vector_pair_uint64_uint64",
    "vector_pair_double_double",

    # Spans
    "span_undirected_edge",
    "span_directed_edge",
    "span_undirected_temporal_edge",
    "span_directed_temporal_edge",
    "span_directed_delayed_temporal_edge",
    "span_undirected_hyperedge",
    "span_directed_hyperedge",
    "span_undirected_temporal_hyperedge",
    "span_directed_temporal_hyperedge",
    "span_directed_delayed_temporal_hyperedge",
    "span_uint64",
    "span_int64",
    "span_uint32",
    "span_int32",
    "span_double",
    "span_pair_uint64_uint64",
    "span_pair_double_double",

    # Operations
    # - algebraic
    "graph_union",
    "disjoint_union",
    "graph_intersection",
    "complement_graph",
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
    "square_grid_graph",
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

    # algorithms
    # - vertex-degree
    "in_degree",
    "out_degree",
    "incident_degree",
    "degree",
    "in_degree_sequence",
    "out_degree_sequence",
    "incident_degree_sequence",
    "degree_sequence",
    "in_out_degree_pair_sequence",
    # - edge degree
    "edge_in_degree",
    "edge_out_degree",
    "edge_incident_degree",
    "edge_degree",
    "edge_in_degree_sequence",
    "edge_out_degree_sequence",
    "edge_incident_degree_sequence",
    "edge_degree_sequence",
    "edge_in_out_degree_pair_sequence",
    # - properties
    "density",
    "time_window",
    "cause_time_window",
    "effect_time_window",
    "link_timeline",
    "window",
    # - static reachability
    "weakly_connected_components",
    "largest_weakly_connected_component",
    "weakly_connected_component",
    "is_weakly_connected",
    "is_weakly_reachable",
    "in_components",
    "out_components",
    "in_component",
    "out_component",
    "in_component_sizes",
    "out_component_sizes",
    "in_component_size_estimates",
    "out_component_size_estimates",
    "strongly_connected_components",
    "largest_strongly_connected_component",
    "strongly_connected_component",
    "is_strongly_connected",
    "is_strongly_reachable",
    "connected_components",
    "largest_connected_component",
    "connected_component",
    "is_connected",
    "is_reachable",
    # - graphic
    "is_graphic",
    "is_digraphic",
    # - dag
    "is_acyclic",
    "topological_order",
    "try_topological_order",
    # - bipartite
    "is_bipartite",
    "try_two_colouring",
    "two_colouring",
    # - assortativity
    "attribute_assortativity",
    "degree_assortativity",
    "direction",
    # - distance
    "shortest_path_lengths_from",
    "shortest_path_lengths_to",
    "try_diameter",
    "diameter",
    "try_out_eccentricity",
    "out_eccentricity",
    "try_in_eccentricity",
    "in_eccentricity",
    "try_out_radius",
    "out_radius",
    "try_in_radius",
    "in_radius",
    "try_eccentricity",
    "eccentricity",
    "try_radius",
    "radius",
]
