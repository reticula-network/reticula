import reticula as ret


def test_occupy_vertices():
    g = ret.undirected_network([(0, 1), (1, 2), (2, 3)])
    gen = ret.mersenne_twister(42)

    prob_map = {0: 1.0, 1: 0.0, 2: 1.0, 3: 0.0}
    occupied = ret.occupy_vertices(g, prob_map, gen)
    assert set(occupied.vertices()) == {0, 2}
    assert len(occupied.edges()) == 0

    occupied = ret.occupy_vertices(g, lambda x: prob_map[x], gen)
    assert set(occupied.vertices()) == {0, 2}
    assert len(occupied.edges()) == 0

    occupied = ret.occupy_vertices(g, prob_map.get, gen)
    assert set(occupied.vertices()) == {0, 2}
    assert len(occupied.edges()) == 0


def test_occupy_edges():
    g = ret.undirected_network([(0, 1), (1, 2), (2, 3)])
    gen = ret.mersenne_twister(42)

    e = ret.undirected_edge(0, 1)
    prob_map = {ee: 1.0 if ee == e else 0.0 for ee in g.edges()}
    occupied = ret.occupy_edges(g, prob_map, gen)
    assert set(occupied.vertices()) == {0, 1, 2, 3}
    assert occupied.edges() == [e]

    occupied = ret.occupy_edges(g, prob_map.get, gen)
    assert occupied.vertices() == g.vertices()
    assert occupied.edges() == [e]


# @given(any_network)
# def test_uniformly_occupy_vertices(g):
#     gen = ret.mersenne_twister(42)
#
#     occupied = ret.uniformly_occupy_vertices(g, 0.5, gen)
#     assert set(occupied.vertices()).issubset(set(g.vertices()))
#
#     occupied = ret.uniformly_occupy_vertices(g, 0.0, gen)
#     assert occupied.vertices() == []
#     assert occupied.edges() == []
#
#     occupied = ret.uniformly_occupy_vertices(g, 1.0, gen)
#     assert occupied == g
#
#
# @given(any_network)
# def test_uniformly_occupy_edges(g):
#     gen = ret.mersenne_twister(42)
#
#     occupied = ret.uniformly_occupy_edges(g, 0.5, gen)
#     assert len(occupied.edges()) <= len(g.edges())
#     assert occupied.vertices() == g.vertices()
#
#     occupied = ret.uniformly_occupy_edges(g, 0.0, gen)
#     assert occupied.vertices() == g.vertices()
#     assert occupied.edges() == []
#
#     occupied = ret.uniformly_occupy_edges(g, 1.0, gen)
#     assert occupied == g
