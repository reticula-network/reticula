import reticula as ret


def test_path_graph():
    g = ret.path_graph(size=4)
    assert len(g.vertices()) == 4
    assert len(g.edges()) == 3
    assert ret.is_connected(g)


def test_cycle_graph():
    g = ret.cycle_graph(size=5)
    assert len(g.vertices()) == 5
    assert len(g.edges()) == 5


def test_complete_graph():
    g = ret.complete_graph(size=4)
    assert len(g.vertices()) == 4
    assert len(g.edges()) == 6


def test_complete_directed_graph():
    g = ret.complete_directed_graph(size=3)
    assert len(g.vertices()) == 3
    assert len(g.edges()) == 6


def test_random_gnp_graph():
    gen = ret.mersenne_twister(42)
    g = ret.random_gnp_graph(n=5, p=0.5, random_state=gen)
    assert len(g.vertices()) == 5
    assert 0 <= len(g.edges()) <= 10


def test_random_directed_gnp_graph():
    gen = ret.mersenne_twister(42)
    g = ret.random_directed_gnp_graph(n=4, p=0.5, random_state=gen)
    assert len(g.vertices()) == 4
    assert 0 <= len(g.edges()) <= 12


def test_random_regular_graph():
    gen = ret.mersenne_twister(42)
    g = ret.random_regular_graph(size=6, degree=2, random_state=gen)
    assert len(g.vertices()) == 6
    assert ret.degree_sequence(g) == [2]*len(g.vertices())


def test_add_remove_edges_vertices():
    g = ret.path_graph(size=3)
    g2 = ret.with_edges(g, [(0, 2)])
    assert len(g2.edges()) == len(g.edges()) + 1
    g3 = ret.without_edges(g2, [(0, 2)])
    assert g3 == g

    g4 = ret.with_vertices(g, [3, 4])
    assert set(g4.vertices()) == set(g.vertices()) | {3, 4}
    g5 = ret.without_vertices(g4, [3, 4])
    assert g5 == g


def test_subgraph_operations():
    g = ret.cycle_graph(size=4)
    edges = g.edges()[:2]
    sub_e = ret.edge_induced_subgraph(g, edges)
    assert set(sub_e.edges()) == set(edges)

    verts = g.vertices()[:3]
    sub_v = ret.vertex_induced_subgraph(g, verts)
    assert set(sub_v.vertices()) == set(verts)
    for e in sub_v.edges():
        assert set(e.incident_verts()).issubset(set(verts))


def test_graph_union():
    g1 = ret.path_graph(size=3)
    g2 = ret.with_edges(g1, [(0, 2)])
    u = ret.graph_union(g1, g2)
    assert len(u.edges()) == len(g1.edges()) + 1
    assert set(u.vertices()) == set(g2.vertices())


def test_square_grid_graph():
    g = ret.square_grid_graph(dimensions=[3, 3])
    assert len(g.vertices()) == 9
    assert len(g.edges()) == 12


def test_regular_ring_lattice():
    g = ret.regular_ring_lattice(size=6, degree=2)
    assert len(g.vertices()) == 6
    assert ret.degree_sequence(g) == [2]*len(g.vertices())


def test_random_barabasi_albert_graph():
    gen = ret.mersenne_twister(42)
    g = ret.random_barabasi_albert_graph(
        n=10, m=2, random_state=gen)
    assert len(g.vertices()) == 10
    assert len(g.edges()) == 2*(10-2)


def test_random_degree_sequence_graph():
    gen = ret.mersenne_twister(42)
    degree_seq = [2, 2, 2, 2]
    g = ret.degree_sequence_graph(
        degree_sequence=degree_seq, random_state=gen)
    assert len(g.vertices()) == 4
    assert all(ret.degree(g, v) == 2 for v in g.vertices())


def test_random_directed_degree_sequence_graph():
    gen = ret.mersenne_twister(42)
    in_out_seq = [(1, 1), (1, 1), (1, 1)]
    g = ret.directed_degree_sequence_graph(
        in_out_degree_pair_sequence=in_out_seq, random_state=gen)
    assert len(g.vertices()) == 3
    for v in g.vertices():
        assert ret.in_degree(g, v) == 1
        assert ret.out_degree(g, v) == 1


def test_random_expected_degree_sequence_graph():
    gen = ret.mersenne_twister(42)
    weight_seq = [2.0, 2.0, 2.0, 2.0]
    g = ret.expected_degree_sequence_graph(
        weight_sequence=weight_seq, random_state=gen)
    assert len(g.vertices()) == 4


def test_random_directed_expected_degree_sequence_graph():
    gen = ret.mersenne_twister(42)
    in_out_weight_seq = [(1.0, 1.0), (1.0, 1.0), (1.0, 1.0)]
    g = ret.directed_expected_degree_sequence_graph(
        in_out_weight_sequence=in_out_weight_seq, random_state=gen)
    assert len(g.vertices()) == 3


def test_random_uniform_hypergraph():
    gen = ret.mersenne_twister(42)
    g = ret.random_uniform_hypergraph(
        size=5, edge_degree=3, edge_prob=0.5, random_state=gen)
    assert len(g.vertices()) == 5
    for e in g.edges():
        assert len(e.incident_verts()) == 3


def test_random_directed_uniform_hypergraph():
    gen = ret.mersenne_twister(42)
    g = ret.random_directed_uniform_hypergraph(
        size=5, edge_in_degree=2, edge_out_degree=2,
        edge_prob=0.5, random_state=gen)
    assert len(g.vertices()) == 5


def test_random_expected_degree_sequence_hypergraph():
    gen = ret.mersenne_twister(42)
    vertex_weights = [3.0, 3.0, 3.0, 3.0]
    edge_weights = [4.0, 4.0, 4.0]
    g = ret.expected_degree_sequence_hypergraph(
        vertex_weight_sequence=vertex_weights,
        edge_weight_sequence=edge_weights, random_state=gen)
    assert len(g.vertices()) == 4


def test_random_directed_expected_degree_sequence_hypergraph():
    gen = ret.mersenne_twister(42)
    vertex_in_out_weights = [(1.0, 1.0), (1.0, 1.0),
                             (1.0, 1.0)]
    edge_in_out_weights = [(3.0, 3.0)]
    g = ret.directed_expected_degree_sequence_hypergraph(
        vertex_in_out_weight_sequence=vertex_in_out_weights,
        edge_in_out_weight_sequence=edge_in_out_weights,
        random_state=gen)
    assert len(g.vertices()) == 3


def test_random_fully_mixed_temporal_network():
    gen = ret.mersenne_twister(42)
    g = ret.random_fully_mixed_temporal_network(
        n=4, rate=1.0, max_t=10.0, random_state=gen)
    assert len(g.vertices()) == 4
    t0, t1 = ret.time_window(g)
    assert t0 >= 0.0
    assert t1 <= 10.0


def test_random_directed_fully_mixed_temporal_network():
    gen = ret.mersenne_twister(42)
    g = ret.random_directed_fully_mixed_temporal_network(
        n=4, rate=1.0, max_t=10.0, random_state=gen)
    assert len(g.vertices()) == 4
    t0, t1 = ret.time_window(g)
    assert t0 >= 0.0
    assert t1 <= 10.0


# def test_random_link_activation_temporal_network():
#     gen = ret.mersenne_twister(42)
#     static_net = ret.complete_graph(size=3)
#     iet_dist = ret.exponential_distribution[ret.double](lmbda=1.0)
#     g = ret.random_link_activation_temporal_network(
#         base_net=static_net, max_t=5.0, iet_dist=iet_dist, random_state=gen)
#     assert len(g.vertices()) == 3
#     t0, t1 = ret.time_window(g)
#     assert t0 >= 0.0
#     assert t1 <= 5.0
#
#
# def test_random_node_activation_temporal_network():
#     gen = ret.mersenne_twister(42)
#     base_net = ret.complete_graph(size=4)
#     iet_dist = ret.exponential_distribution[ret.double](lmbda=1.0)
#     g = ret.random_node_activation_temporal_network(
#         base_net=base_net, max_t=5.0, iet_dist=iet_dist, random_state=gen)
#     assert len(g.vertices()) == 4
#     t0, t1 = ret.time_window(g)
#     assert t0 >= 0.0
#     assert t1 <= 5.0
