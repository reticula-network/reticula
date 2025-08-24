#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>

#include <reticula/edges/undirected_temporal_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_undirected_temporal_edge(nanobind::module_& m) {
  nanobind::class_<undirected_temporal_edge>(m, "undirected_temporal_edge")
    .def(nanobind::init<VertexType, VertexType, double>())
    .def(
      "__repr__",
      [](const undirected_temporal_edge& e) {
        return "undirected_temporal_edge(" +
               std::to_string(e.incident_verts().front()) + ", " +
               std::to_string(e.incident_verts().back()) + ", " +
               std::to_string(e.cause_time()) + ")";
      })
    .def("cause_time", &undirected_temporal_edge::cause_time)
    .def("effect_time", &undirected_temporal_edge::effect_time)
    .def(edge_properties());
}
} // namespace reticula::python
