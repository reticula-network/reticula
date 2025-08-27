#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <reticula/edges/directed_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_hyperedge(nanobind::module_& m) {
  nanobind::class_<directed_hyperedge>(m, "directed_hyperedge")
    .def(nanobind::init<std::vector<VertexType>, std::vector<VertexType>>())
    .def(
      "__repr__",
      [](const directed_hyperedge& e) {
        return "directed_hyperedge(" + reticula::utils::join(e.tails(), ", ") +
               ", " + reticula::utils::join(e.heads(), ", ") + ")";
      })
    .def(
      "tails", &directed_hyperedge::tails,
      nanobind::rv_policy::reference_internal)
    .def(
      "heads", &directed_hyperedge::heads,
      nanobind::rv_policy::reference_internal)
    .def(edge_properties());
}
} // namespace reticula::python
