import reticula as ret


def test_temporal_edge_creation():
    edge = ret.undirected_temporal_edge(0, 1, 1.5)
    assert edge.incident_verts() == [0, 1]
    assert edge.mutator_verts() == [0, 1]
    assert edge.mutated_verts() == [0, 1]
    assert edge.cause_time() == 1.5
    assert edge.effect_time() == 1.5

    edge = ret.directed_temporal_edge(0, 1, 2.0)
    assert edge.incident_verts() == [0, 1]
    assert edge.mutator_verts() == [0]
    assert edge.mutated_verts() == [1]
    assert edge.cause_time() == 2.0
    assert edge.effect_time() == 2.0

    edge = ret.directed_delayed_temporal_edge(0, 1, 1.0, 2.0)
    assert edge.incident_verts() == [0, 1]
    assert edge.mutator_verts() == [0]
    assert edge.mutated_verts() == [1]
    assert edge.cause_time() == 1.0
    assert edge.effect_time() == 2.0


def test_temporal_network_creation():
    edges = [
        ret.undirected_temporal_edge(0, 1, 1.0),
        ret.undirected_temporal_edge(1, 2, 2.0)
    ]
    net = ret.undirected_temporal_network(edges)
    assert sorted(net.edges()) == sorted(edges)
    assert net.vertices() == [0, 1, 2]

    net = ret.undirected_temporal_network(
        edges, verts=[5])
    assert sorted(net.edges()) == sorted(edges)
    assert net.vertices() == [0, 1, 2, 5]


def test_temporal_hyperedge_creation():
    edge = ret.undirected_temporal_hyperedge([0, 1, 2], 1.5)
    assert edge.incident_verts() == [0, 1, 2]
    assert edge.mutator_verts() == [0, 1, 2]
    assert edge.mutated_verts() == [0, 1, 2]
    assert edge.cause_time() == 1.5
    assert edge.effect_time() == 1.5

    edge = ret.directed_temporal_hyperedge([0, 1], [2, 3], 2.0)
    assert edge.incident_verts() == [0, 1, 2, 3]
    assert edge.mutated_verts() == [2, 3]
    assert edge.mutator_verts() == [0, 1]
    assert edge.cause_time() == 2.0
    assert edge.effect_time() == 2.0

    edge = ret.directed_delayed_temporal_hyperedge([0], [1, 2], 1.0, 2.0)
    assert edge.incident_verts() == [0, 1, 2]
    assert edge.mutated_verts() == [1, 2]
    assert edge.mutator_verts() == [0]
    assert edge.cause_time() == 1.0
    assert edge.effect_time() == 2.0


def test_temporal_hypernetwork_creation():
    edges = [
        ret.undirected_temporal_hyperedge([0, 1, 2], 1.0),
        ret.undirected_temporal_hyperedge([1, 2, 3], 2.0)
    ]
    net = ret.undirected_temporal_hypernetwork(edges)
    assert sorted(net.edges()) == sorted(edges)
    assert net.vertices() == [0, 1, 2, 3]

    edges = [
        ret.directed_temporal_hyperedge([0, 1], [2], 1.),
        ret.directed_temporal_hyperedge([2], [3], 2.)
    ]
    net = ret.directed_temporal_hypernetwork(edges, verts=[5])
    assert sorted(net.edges()) == sorted(edges)
    assert net.vertices() == [0, 1, 2, 3, 5]
