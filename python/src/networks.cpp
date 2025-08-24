#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

#include "spans.hpp"

namespace reticula::python {
namespace {
template <network_edge EdgeT>
void define_network(nanobind::module_& m, const std::string& name) {
  using NetT = network<EdgeT>;
  nanobind::class_<NetT>(m, name.c_str())
    .def(nanobind::init<>())
    .def(nanobind::init<std::vector<EdgeT>>())
    .def(nanobind::init<std::vector<EdgeT>, std::vector<VertexType>>())
    .def(nanobind::self != nanobind::self)
    .def(nanobind::self == nanobind::self)
    .def(
      "__repr__",
      [name](const NetT& net) {
        return "<" + name + " with " + std::to_string(net.edges().size()) +
               " edges and " + std::to_string(net.vertices().size()) +
               " vertices>";
      })
    .def("edges", &NetT::edges)
    .def("edges_cause", &NetT::edges_cause)
    .def("edges_effect", &NetT::edges_effect)
    .def(
      "vertices", utils::ndarray_output<&NetT::vertices>(),
      nanobind::rv_policy::reference_internal)
    .def("incident_edges", &NetT::incident_edges)
    .def("out_edges", &NetT::out_edges)
    .def("in_edges", &NetT::in_edges)
    .def("successors", utils::ndarray_output<&NetT::successors>())
    .def("predecessors", utils::ndarray_output<&NetT::predecessors>())
    .def("neighbours", utils::ndarray_output<&NetT::neighbours>());
}
} // namespace

void define_networks(nanobind::module_& m) {
  define_network<undirected_edge>(m, "undirected_network");
  define_network<directed_edge>(m, "directed_network");
  define_network<undirected_temporal_edge>(m, "undirected_temporal_network");
  define_network<directed_temporal_edge>(m, "directed_temporal_network");
  define_network<directed_delayed_temporal_edge>(
    m, "directed_delayed_temporal_network");

  define_network<undirected_hyperedge>(m, "undirected_hypernetwork");
  define_network<directed_hyperedge>(m, "directed_hypernetwork");
  define_network<undirected_temporal_hyperedge>(
    m, "undirected_temporal_hypernetwork");
  define_network<directed_temporal_hyperedge>(
    m, "directed_temporal_hypernetwork");
  define_network<directed_delayed_temporal_hyperedge>(
    m, "directed_delayed_temporal_hypernetwork");
}
} // namespace reticula::python
