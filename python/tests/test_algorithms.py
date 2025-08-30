import reticula as ret

import pytest


def test_density():
    g = ret.complete_graph(size=4)
    assert ret.density(g) == 1.0

    g = ret.undirected_network(edges=[], verts=range(4))
    assert ret.density(g) == 0.0

    g = ret.path_graph(size=4)
    expected_density = 3 / 6
    assert ret.density(g) == pytest.approx(expected_density)


def test_is_connected():
    g = ret.path_graph(size=4)
    assert ret.is_connected(g)

    g = ret.undirected_network([(0, 1), (2, 3)])
    assert not ret.is_connected(g)

    g = ret.undirected_network([], [0])
    assert ret.is_connected(g)

    g = ret.undirected_network()
    assert ret.is_connected(g)


def test_is_weakly_connected():
    g = ret.directed_network([(0, 1), (1, 2)])
    assert ret.is_weakly_connected(g)

    g = ret.directed_network([(0, 1), (2, 3)])
    assert not ret.is_weakly_connected(g)


def test_is_acyclic():
    g = ret.directed_network([(0, 1), (0, 2), (1, 3), (2, 3)])
    assert ret.is_acyclic(g)

    g = ret.directed_network([(0, 1), (1, 2), (2, 0)])
    assert not ret.is_acyclic(g)


def test_topological_order():
    g = ret.directed_network([(0, 1), (0, 2), (1, 3), (2, 3)])
    order = list(ret.topological_order(g))
    assert len(order) == 4
    assert order.index(0) < order.index(1)
    assert order.index(0) < order.index(2)
    assert order.index(1) < order.index(3)
    assert order.index(2) < order.index(3)

    g = ret.directed_network([(0, 1), (1, 2), (2, 0)])
    pytest.raises(ValueError, ret.topological_order, g)
    assert ret.try_topological_order(g) is None


def test_graphicallity():
    ds = [16, 9, 10, 6, 3, 4, 4, 4, 5, 2, 3, 1, 2, 5, 2, 2, 2,
          2, 2, 3, 2, 2, 2, 5, 3, 3, 2, 4, 3, 4, 4, 6, 12, 17]
    assert ret.is_graphic(ds)

    dps = [(1, 8), (3, 3), (3, 2), (2, 5), (5, 3), (4, 3), (3, 2),
           (4, 4), (4, 1), (3, 0), (4, 3), (3, 5), (3, 1), (3, 3), (0, 2)]
    assert ret.is_digraphic(dps)

# def test_shortest_path_lengths():
#     g = ret.path_graph(size=4)
#
#     distances_from = ret.shortest_path_lengths_from(g, 0)
#     assert distances_from[0] == 0
#     assert distances_from[1] == 1
#     assert distances_from[2] == 2
#     assert distances_from[3] == 3
#     assert ret.shortest_path_lengths_from(g, 12) == {12: 0}
#
#     distances_to = ret.shortest_path_lengths_to(g, 3)
#     assert distances_to[0] == 3
#     assert distances_to[1] == 2
#     assert distances_to[2] == 1
#     assert distances_to[3] == 0
#     assert ret.shortest_path_lengths_to(g, 12) == {12: 0}


def test_is_reachable():
    g = ret.path_graph(size=4)
    assert ret.is_reachable(g, 0, 3)
    assert ret.is_reachable(g, 3, 0)

    g = ret.undirected_network([(0, 1), (2, 3)])
    assert not ret.is_reachable(g, 0, 2)


def test_component_functions():
    g = ret.undirected_network([(0, 1), (2, 3), (4, 5), (5, 6)])

    comp = ret.connected_component(g, 0)
    assert set(comp) == {0, 1}
    assert len(comp) == 2

    comp = ret.largest_connected_component(g)
    assert set(comp) == {4, 5, 6}

    all_comps = ret.connected_components(g)
    assert len(all_comps) == 3


def test_weakly_connected_components():
    g = ret.directed_network([(0, 1), (2, 3)])
    wccs = ret.weakly_connected_components(g)
    assert len(wccs) == 2

    wcc_sets = [set(comp) for comp in wccs]
    assert {0, 1} in wcc_sets
    assert {2, 3} in wcc_sets

    lwcc = ret.largest_weakly_connected_component(g)
    assert len(lwcc) == 2


def test_in_out_components():
    g = ret.directed_network([(0, 1), (1, 2), (3, 1)])

    in_comp = ret.in_component(g, 1)
    assert 0 in in_comp
    assert 3 in in_comp

    out_comp = ret.out_component(g, 1)
    assert 2 in out_comp

    in_sizes = ret.in_component_sizes(g)
    assert len(in_sizes) == len(g.vertices())

    out_sizes = ret.out_component_sizes(g)
    assert len(out_sizes) == len(g.vertices())


def test_degree_sequence():
    g = ret.undirected_network([(0, 1), (1, 2), (2, 3)])

    deg_seq = ret.degree_sequence(g)
    assert set(deg_seq) == {1, 2, 2, 1}

    g = ret.directed_network([(0, 1), (0, 2), (1, 2)])
    in_deg_seq = ret.in_degree_sequence(g)
    assert set(in_deg_seq) == {0, 1, 2}

    out_deg_seq = ret.out_degree_sequence(g)
    assert set(out_deg_seq) == {2, 1, 0}

    in_out_deg_seq = ret.in_out_degree_pair_sequence(g)
    assert set(in_out_deg_seq) == {(0, 2), (1, 1), (2, 0)}

    inc_deg_seq = ret.incident_degree_sequence(g)
    assert set(inc_deg_seq) == {2, 2, 2}


def test_bipartite():
    g = ret.undirected_network([(0, 3), (1, 3), (1, 4), (2, 4)], verts=range(5))
    assert ret.is_bipartite(g)
    assert ret.try_two_colouring(g) is not None
    parts = ret.two_colouring(g)
    assert set(frozenset(p) for p in parts) == {
        frozenset({0, 1, 2}), frozenset({3, 4})}

    g = ret.directed_delayed_temporal_network(
        [(0, 3, 1, 2), (1, 3, 5, 7), (1, 4, 1, 3), (2, 4, 9, 12)],
        verts=range(5))

    assert ret.is_bipartite(g)
    assert ret.try_two_colouring(g) is not None
    parts = ret.two_colouring(g)
    assert set(frozenset(p) for p in parts) == {
        frozenset({0, 1, 2}), frozenset({3, 4})}

# @given(any_undirected_network)
# def test_degree_properties(net):
#     for v in net.vertices():
#         deg = ret.degree(net, v)
#         assert deg == len([e for e in net.edges() if v in e.incident_verts()])


# @given(any_network)
# def test_directed_degree_properties(net):
#     for v in net.vertices():
#         in_deg = ret.in_degree(net, v)
#         assert in_deg == len([
#             e for e in net.edges()
#             if v in e.mutated_verts()])
#
#         out_deg = ret.out_degree(net, v)
#         assert out_deg == len([
#             e for e in net.edges()
#             if v in e.mutator_verts()])
#
#         inc_deg = ret.incident_degree(net, v)
#         assert inc_deg == len([
#             e for e in net.edges()
#             if v in e.incident_verts()])
#
#     for v in net.vertices():
#         assert ret.in_degree(net, v) >= 0
#         assert ret.out_degree(net, v) >= 0


# def test_attribute_assortativity():
#     g = ret.undirected_network([(0, 1), (1, 2), (2, 3)])
#     attrs = {0: 1.0, 1: 2.0, 2: 2.0, 3: 1.0}
#
#     r = ret.attribute_assortativity(g, attrs, 0.0)
#     assert isinstance(r, float)
#     assert r == pytest.approx(-0.5)
#
#     r = ret.attribute_assortativity(g, {}, 1.0)
#     assert math.isnan(r)
