#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>

#include <reticula/edges/undirected_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_undirected_edge(nanobind::module_& m) {
  nanobind::class_<undirected_edge>(m, "undirected_edge")
    .def(nanobind::init<VertexType, VertexType>())
    .def(
      "__repr__",
      [](const undirected_edge& e) {
        return "undirected_edge(" + std::to_string(e.incident_verts().front()) +
               ", " + std::to_string(e.incident_verts().back()) + ")";
      })
  .def(edge_properties());
}
} // namespace reticula::python
