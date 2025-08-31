#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/directed_temporal_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_temporal_hyperedge(nanobind::module_& m) {
  nanobind::class_<directed_temporal_hyperedge>(
    m, "directed_temporal_hyperedge")
    .def(
      nanobind::init<
        std::vector<VertexType>, std::vector<VertexType>, double>(),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__init__",
      [](
        directed_temporal_hyperedge* e,
        std::tuple<std::vector<VertexType>, std::vector<VertexType>, double>
          t) {
        new (e) directed_temporal_hyperedge(
          std::get<0>(t), std::get<1>(t), std::get<2>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const directed_temporal_hyperedge& e) {
        return "directed_temporal_hyperedge(" +
               reticula::utils::join(e.tails(), ", ") + ", " +
               reticula::utils::join(e.heads(), ", ") + ", " +
               std::to_string(e.cause_time()) + ", " +
               std::to_string(e.effect_time()) + ")";
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "tails", &directed_temporal_hyperedge::tails,
      nanobind::rv_policy::reference_internal,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "heads", &directed_temporal_hyperedge::heads,
      nanobind::rv_policy::reference_internal,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("cause_time", &directed_temporal_hyperedge::cause_time,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("effect_time", &directed_temporal_hyperedge::effect_time,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<std::vector<VertexType>, std::vector<VertexType>, double>,
    directed_temporal_hyperedge>();
}
} // namespace reticula::python
