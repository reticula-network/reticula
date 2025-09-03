#include <nanobind/nanobind.h>

#include <reticula/edges.hpp>
#include <reticula/temporal_adjacency.hpp>

namespace reticula::python {
namespace {
template <typename AdjT>
void define_adjacency_linger(nanobind::class_<AdjT>& adj) {
  adj
    .def(
      "linger", &AdjT::template linger<undirected_temporal_edge>,
      nanobind::arg("edge"), nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "linger", &AdjT::template linger<directed_temporal_edge>,
      nanobind::arg("edge"), nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "linger", &AdjT::template linger<directed_delayed_temporal_edge>,
      nanobind::arg("edge"), nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "linger", &AdjT::template linger<undirected_temporal_hyperedge>,
      nanobind::arg("edge"), nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "linger", &AdjT::template linger<directed_temporal_hyperedge>,
      nanobind::arg("edge"), nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "linger", &AdjT::template linger<directed_delayed_temporal_hyperedge>,
      nanobind::arg("edge"), nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_adjacency(nanobind::module_& m) {
  auto adj = m.def_submodule("adjacency");
  auto simple = nanobind::class_<adjacency::simple>(adj, "simple")
                  .def(nanobind::init<>())
                  .def(
                    "maximum_linger", &adjacency::simple::maximum_linger,
                    nanobind::arg("vertex"),
                    nanobind::call_guard<nanobind::gil_scoped_release>());
  define_adjacency_linger(simple);

  auto lwt =
    nanobind::class_<adjacency::limited_waiting_time>(
      adj, "limited_waiting_time")
      .def(
        nanobind::init<double>(), nanobind::arg("dt"),
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "dt", &adjacency::limited_waiting_time::dt,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "maximum_linger", &adjacency::limited_waiting_time::maximum_linger,
        nanobind::arg("vertex"),
        nanobind::call_guard<nanobind::gil_scoped_release>());
  define_adjacency_linger(lwt);

  auto exp = nanobind::class_<adjacency::exponential>(adj, "exponential")
               .def(
                 nanobind::init<double, std::size_t>(), nanobind::arg("rate"),
                 nanobind::arg("seed") = 0,
                 nanobind::call_guard<nanobind::gil_scoped_release>())
               .def(
                 "rate", &adjacency::exponential::rate,
                 nanobind::call_guard<nanobind::gil_scoped_release>())
               .def(
                 "seed", &adjacency::exponential::seed,
                 nanobind::call_guard<nanobind::gil_scoped_release>())
               .def(
                 "maximum_linger", &adjacency::exponential::maximum_linger,
                 nanobind::arg("vertex"),
                 nanobind::call_guard<nanobind::gil_scoped_release>());
  define_adjacency_linger(exp);

  auto geom = nanobind::class_<adjacency::geometric>(adj, "geometric")
                .def(
                  nanobind::init<double, std::size_t>(), nanobind::arg("p"),
                  nanobind::arg("seed") = 0,
                  nanobind::call_guard<nanobind::gil_scoped_release>())
                .def(
                  "p", &adjacency::geometric::p,
                  nanobind::call_guard<nanobind::gil_scoped_release>())
                .def(
                  "seed", &adjacency::geometric::seed,
                  nanobind::call_guard<nanobind::gil_scoped_release>())
                .def(
                  "maximum_linger", &adjacency::geometric::maximum_linger,
                  nanobind::arg("vertex"),
                  nanobind::call_guard<nanobind::gil_scoped_release>());
  define_adjacency_linger(geom);
}
} // namespace reticula::python
