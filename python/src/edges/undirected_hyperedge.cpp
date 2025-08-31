#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/undirected_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_undirected_hyperedge(nanobind::module_& m) {
  nanobind::class_<undirected_hyperedge>(m, "undirected_hyperedge")
    .def(
      nanobind::init<std::vector<VertexType>>(),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const undirected_hyperedge& e) {
        return "undirected_hyperedge(" +
               reticula::utils::join(e.incident_verts(), ", ") + ")";
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::vector<VertexType>, undirected_hyperedge>();
}
} // namespace reticula::python
