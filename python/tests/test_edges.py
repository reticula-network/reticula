import reticula as ret

from numpy.testing import assert_array_equal


def test_temporal_edge_creation():
    edge = ret.undirected_temporal_edge(0, 1, 1.5)
    assert_array_equal(edge.incident_verts(), [0, 1])
    assert_array_equal(edge.mutator_verts(), [0, 1])
    assert_array_equal(edge.mutated_verts(), [0, 1])
    assert edge.cause_time() == 1.5
    assert edge.effect_time() == 1.5

    edge = ret.directed_temporal_edge(0, 1, 2.0)
    assert_array_equal(edge.incident_verts(), [0, 1])
    assert_array_equal(edge.mutator_verts(), [0])
    assert_array_equal(edge.mutated_verts(), [1])
    assert edge.cause_time() == 2.0
    assert edge.effect_time() == 2.0

    edge = ret.directed_delayed_temporal_edge(0, 1, 1.0, 2.0)
    assert_array_equal(edge.incident_verts(), [0, 1])
    assert_array_equal(edge.mutator_verts(), [0])
    assert_array_equal(edge.mutated_verts(), [1])
    assert edge.cause_time() == 1.0
    assert edge.effect_time() == 2.0


def test_temporal_network_creation():
    edges = [
        ret.undirected_temporal_edge(0, 1, 1.0),
        ret.undirected_temporal_edge(1, 2, 2.0)
    ]
    net = ret.undirected_temporal_network(edges)
    # assert len(net.edges()) == 2
    assert len(net.vertices()) == 3

    net = ret.undirected_temporal_network(
        edges, verts=[5])
    # assert len(net.edges()) == 2
    assert len(net.vertices()) == 4


# def test_temporal_hyperedge_creation():
#     edge = ret.undirected_temporal_hyperedge([0, 1, 2], 1.5)
#     assert_array_equal(edge.incident_verts(), [0, 1, 2])
#     assert_array_equal(edge.mutator_verts(), [0, 1, 2])
#     assert_array_equal(edge.mutated_verts(), [0, 1, 2])
#     assert edge.cause_time() == 1.5
#     assert edge.effect_time() == 1.5
#
#     edge = ret.directed_temporal_hyperedge([0, 1], [2, 3], 2.0)
#     assert_array_equal(edge.incident_verts(), [0, 1, 2, 3])
#     assert_array_equal(edge.mutated_verts(), [2, 3])
#     assert_array_equal(edge.mutator_verts(), [0, 1])
#     assert edge.cause_time() == 2.0
#     assert edge.effect_time() == 2.0
#
#     edge = ret.directed_delayed_temporal_hyperedge([0], [1, 2], 1.0, 2.0)
#     assert_array_equal(edge.incident_verts(), [0, 1, 2])
#     assert_array_equal(edge.mutated_verts(), [1, 2])
#     assert_array_equal(edge.mutator_verts(), [0])
#     assert edge.cause_time() == 1.0
#     assert edge.effect_time() == 2.0
#
#
# def test_temporal_hypernetwork_creation():
#     edges = [
#         ret.undirected_temporal_hyperedge([0, 1, 2], 1.0),
#         ret.undirected_temporal_hyperedge([1, 2, 3], 2.0)
#     ]
#     net = ret.undirected_temporal_hypernetwork(edges)
#     assert len(net.edges()) == 2
#     assert len(net.vertices()) == 4
