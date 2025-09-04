#include <nanobind/nanobind.h>

#include <reticula/concepts.hpp>
#include <reticula/edges.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_edge EdgeT>
void define_concepts_for_edge(nanobind::module_& m) {
  m.def(
    "is_instantaneous", [](const EdgeT&) { return is_instantaneous_v<EdgeT>; });
  m.def("is_undirected", [](const EdgeT&) { return is_undirected_v<EdgeT>; });
  m.def("is_directed", [](const EdgeT&) { return is_directed_v<EdgeT>; });
  m.def("is_dyadic", [](const EdgeT&) { return is_dyadic_v<EdgeT>; });
  m.def("is_static", [](const EdgeT&) { return static_network_edge<EdgeT>; });
  m.def(
    "is_temporal", [](const EdgeT&) { return temporal_network_edge<EdgeT>; });
}

template <network_like NetT>
void define_concepts_for_network(nanobind::module_& m) {
  m.def(
    "is_instantaneous", [](const NetT&) { return is_instantaneous_v<NetT>; });
  m.def("is_undirected", [](const NetT&) { return is_undirected_v<NetT>; });
  m.def("is_directed", [](const NetT&) { return is_directed_v<NetT>; });
  m.def("is_dyadic", [](const NetT&) { return is_dyadic_v<NetT>; });
  m.def("is_static", [](const NetT&) {
    return static_network_edge<typename NetT::EdgeType>;
  });
  m.def("is_temporal", [](const NetT&) {
    return temporal_network_edge<typename NetT::EdgeType>;
  });
}
} // namespace

void define_concepts(nanobind::module_& m) {
  define_concepts_for_edge<undirected_edge>(m);
  define_concepts_for_edge<directed_edge>(m);
  define_concepts_for_edge<undirected_temporal_edge>(m);
  define_concepts_for_edge<directed_temporal_edge>(m);
  define_concepts_for_edge<directed_delayed_temporal_edge>(m);

  define_concepts_for_edge<undirected_hyperedge>(m);
  define_concepts_for_edge<directed_hyperedge>(m);
  define_concepts_for_edge<undirected_temporal_hyperedge>(m);
  define_concepts_for_edge<directed_temporal_hyperedge>(m);
  define_concepts_for_edge<directed_delayed_temporal_hyperedge>(m);

  define_concepts_for_network<undirected_network>(m);
  define_concepts_for_network<directed_network>(m);
  define_concepts_for_network<undirected_temporal_network>(m);
  define_concepts_for_network<directed_temporal_network>(m);
  define_concepts_for_network<directed_delayed_temporal_network>(m);

  define_concepts_for_network<undirected_hypernetwork>(m);
  define_concepts_for_network<directed_hypernetwork>(m);
  define_concepts_for_network<undirected_temporal_hypernetwork>(m);
  define_concepts_for_network<directed_temporal_hypernetwork>(m);
  define_concepts_for_network<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
