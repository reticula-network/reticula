#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/directed_delayed_temporal_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_delayed_temporal_hyperedge(nanobind::module_& m) {
  nanobind::class_<directed_delayed_temporal_hyperedge>(
    m, "directed_delayed_temporal_hyperedge")
    .def(
      nanobind::init<
        std::vector<VertexType>, std::vector<VertexType>, double, double>())
    .def(
      "__init__",
      [](
        directed_delayed_temporal_hyperedge* e,
        std::tuple<
          std::vector<VertexType>, std::vector<VertexType>, double, double>
          t) {
        new (e) directed_delayed_temporal_hyperedge(
          std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const directed_delayed_temporal_hyperedge& e) {
        return "directed_delayed_temporal_hyperedge(" +
               reticula::utils::join(e.tails(), ", ") + ", " +
               reticula::utils::join(e.heads(), ", ") + ", " +
               std::to_string(e.cause_time()) + ", " +
               std::to_string(e.effect_time()) + ")";
      })
    .def(
      "tails", &directed_delayed_temporal_hyperedge::tails,
      nanobind::rv_policy::reference_internal)
    .def(
      "heads", &directed_delayed_temporal_hyperedge::heads,
      nanobind::rv_policy::reference_internal)
    .def("cause_time", &directed_delayed_temporal_hyperedge::cause_time)
    .def("effect_time", &directed_delayed_temporal_hyperedge::effect_time)
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<
      std::vector<VertexType>, std::vector<VertexType>, double, double>,
    directed_delayed_temporal_hyperedge>();
}
} // namespace reticula::python
