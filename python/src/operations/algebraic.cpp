#include <nanobind/nanobind.h>

#include <reticula/operations/algebraic.hpp>

namespace reticula::python {
namespace {
template <network_like NetT1, network_like NetT2 = NetT1>
  requires std::is_same_v<typename NetT1::EdgeType, typename NetT2::EdgeType>
void define_algebraic_for_nets(nanobind::module_& m) {
  m.def(
    "graph_union", reticula::graph_union<NetT1, NetT2>, nanobind::arg("g1"),
    nanobind::arg("g2"));

  m.def(
    "disjoint_union", reticula::disjoint_union<NetT1, NetT2>,
    nanobind::arg("g1"), nanobind::arg("g2"));

  m.def(
    "graph_intersection", reticula::graph_intersection<NetT1, NetT2>,
    nanobind::arg("g1"), nanobind::arg("g2"));
}

// template <network_like NetT1, network_like NetT2>
// void define_product_for_nets(nanobind::module_& m) {
//   m.def(
//     "cartesian_product", &reticula::cartesian_product<NetT1, NetT2>,
//     nanobind::arg("g1"), nanobind::arg("g2"));
// }

template <static_network_like NetT>
void define_complement(nanobind::module_& m) {
  m.def(
    "complement_graph", &reticula::complement_graph<NetT>, nanobind::arg("g"));
}
} // namespace

void define_algebraic(nanobind::module_& m) {
  define_algebraic_for_nets<undirected_network>(m);
  define_algebraic_for_nets<directed_network>(m);
  define_algebraic_for_nets<undirected_temporal_network>(m);
  define_algebraic_for_nets<directed_temporal_network>(m);
  define_algebraic_for_nets<directed_delayed_temporal_network>(m);

  define_algebraic_for_nets<undirected_hypernetwork>(m);
  define_algebraic_for_nets<directed_hypernetwork>(m);
  define_algebraic_for_nets<undirected_temporal_hypernetwork>(m);
  define_algebraic_for_nets<directed_temporal_hypernetwork>(m);
  define_algebraic_for_nets<directed_delayed_temporal_hypernetwork>(m);

  define_complement<undirected_network>(m);
  define_complement<directed_network>(m);
}
} // namespace reticula::python
