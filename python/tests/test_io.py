import reticula as ret

import networkx as nx


def test_undirected_to_from_networkx():
    g = ret.undirected_network([(0, 1), (1, 2), (2, 3), (3, 0)], verts=[15])
    nx_g = ret.to_networkx(g)
    assert isinstance(nx_g, nx.Graph)

    g_back, _ = ret.from_networkx(nx_g)
    assert g_back == g


def test_directed_to_from_networkx():
    g = ret.directed_network([(0, 1), (1, 2), (2, 3), (3, 0)], verts=[15])
    nx_g = ret.to_networkx(g)
    assert isinstance(nx_g, nx.DiGraph)

    g_back, _ = ret.from_networkx(nx_g)
    assert g_back == g


def test_undirected_to_from_networkx_nonint():
    nx_g = nx.DiGraph([("a", "b"), ("b", "c"), ("c", "d"), ("d", "a")])
    nx_g.add_node("e")  # isolated node
    g_back, vert_map = ret.from_networkx(nx_g)

    assert vert_map == {"a": 0, "b": 1, "c": 2, "d": 3, "e": 4}
    assert g_back == ret.directed_network(
        [(0, 1), (1, 2), (2, 3), (3, 0)], verts=[4])
