#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/directed_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_hyperedge(nanobind::module_& m) {
  nanobind::class_<directed_hyperedge>(m, "directed_hyperedge")
    .def(nanobind::init<std::vector<VertexType>, std::vector<VertexType>>())
    .def(
      "__init__",
      [](
        directed_hyperedge* e,
        std::tuple<std::vector<VertexType>, std::vector<VertexType>> t) {
        new (e) directed_hyperedge(std::get<0>(t), std::get<1>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
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

  nanobind::implicitly_convertible<
    std::tuple<std::vector<VertexType>, std::vector<VertexType>>,
    directed_hyperedge>();
}
} // namespace reticula::python
