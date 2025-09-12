#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>

#include <reticula/algorithms/properties.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_properties_for_net(nanobind::module_& m) {
  if constexpr (
    is_dyadic_v<NetT> && static_network_edge<typename NetT::EdgeType>)
    m.def("density", &density<NetT>, nanobind::arg("net"));

  if constexpr (temporal_network_edge<typename NetT::EdgeType>) {
    m.def("time_window", &time_window<NetT>, nanobind::arg("net"));
    m.def("cause_time_window", &cause_time_window<NetT>, nanobind::arg("net"));
    m.def(
      "effect_time_window", &effect_time_window<NetT>, nanobind::arg("net"));
    m.def(
      "link_timeline", &link_timeline<NetT>, nanobind::arg("net"),
      nanobind::arg("link"));
  }
}
} // namespace

void define_properties(nanobind::module_& m) {
  nanobind::class_<window>(m, "window")
    .def_ro("start", &window::start)
    .def_ro("end", &window::end)
    .def("duration", &window::duration)
    .def("duration_is_valid", &window::duration_is_valid)
    .def(
      "contains",
      nanobind::overload_cast<TimeType>(&window::contains, nanobind::const_),
      nanobind::arg("t"))
    .def(
      "contains",
      nanobind::overload_cast<const window&>(
        &window::contains, nanobind::const_),
      nanobind::arg("t"))
    .def("overlaps", &window::overlaps, nanobind::arg("other"))
    .def(
      "intersection_with", &window::intersection_with, nanobind::arg("other"))
    .def("union_with", &window::union_with, nanobind::arg("other"))
    .def(nanobind::self == nanobind::self)
    .def(
      "__repr__",
      [](const window& w) {
        return std::string(
          "window{start: " + std::to_string(w.start) +
          ", end: " + std::to_string(w.end) + "}");
      })
    .def(
      "__iter__",
      [](const window& w) {
        return nanobind::iter(nanobind::make_tuple(w.start, w.end));
      });

  define_properties_for_net<undirected_network>(m);
  define_properties_for_net<directed_network>(m);
  define_properties_for_net<undirected_temporal_network>(m);
  define_properties_for_net<directed_temporal_network>(m);
  define_properties_for_net<directed_delayed_temporal_network>(m);

  define_properties_for_net<undirected_hypernetwork>(m);
  define_properties_for_net<directed_hypernetwork>(m);
  define_properties_for_net<undirected_temporal_hypernetwork>(m);
  define_properties_for_net<directed_temporal_hypernetwork>(m);
  define_properties_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
