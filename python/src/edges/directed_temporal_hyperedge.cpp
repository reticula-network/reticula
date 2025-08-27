#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <reticula/edges/directed_temporal_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_temporal_hyperedge(nanobind::module_& m) {
  nanobind::class_<directed_temporal_hyperedge>(
    m, "directed_temporal_hyperedge")
    .def(
      nanobind::init<
        std::vector<VertexType>, std::vector<VertexType>, double>())
    .def(
      "__repr__",
      [](const directed_temporal_hyperedge& e) {
        return "directed_temporal_hyperedge(" +
               reticula::utils::join(e.tails(), ", ") + ", " +
               reticula::utils::join(e.heads(), ", ") + ", " +
               std::to_string(e.cause_time()) + ", " +
               std::to_string(e.effect_time()) + ")";
      })
    .def("tails", &directed_temporal_hyperedge::tails,
         nanobind::rv_policy::reference_internal)
    .def("heads", &directed_temporal_hyperedge::heads,
         nanobind::rv_policy::reference_internal)
    .def("cause_time", &directed_temporal_hyperedge::cause_time)
    .def("effect_time", &directed_temporal_hyperedge::effect_time)
    .def(edge_properties());
}
} // namespace reticula::python
