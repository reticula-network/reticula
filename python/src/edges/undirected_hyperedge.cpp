#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include <reticula/edges/undirected_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_undirected_hyperedge(nanobind::module_& m) {
  nanobind::class_<undirected_hyperedge>(m, "undirected_hyperedge")
    .def(nanobind::init<std::vector<VertexType>>())
    .def(
      "__repr__",
      [](const undirected_hyperedge& e) {
        return "undirected_hyperedge(" +
               reticula::utils::join(e.incident_verts(), ", ") + ")";
      })
    .def(edge_properties());
}
} // namespace reticula::python
