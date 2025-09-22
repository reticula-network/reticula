import sys

import reticula as ret


def test_edges_span_references_creation():
    g = ret.directed_network([(0, 1), (1, 2)])
    refs = sys.getrefcount(g)

    e = g.edges()
    refs_after = sys.getrefcount(g)

    assert refs == refs_after - 1
    assert len(e) == 2
