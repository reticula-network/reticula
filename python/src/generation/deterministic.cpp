#include <nanobind/nanobind.h>

#include <reticula/generation/deterministic.hpp>

namespace reticula::python {
void define_deterministic(nanobind::module_& m) {
  m.def(
    "path_graph", &reticula::path_graph, nanobind::arg("size"),
    nanobind::arg("periodic") = false);

  m.def("cycle_graph", &reticula::cycle_graph, nanobind::arg("size"));

  m.def("star_graph", &reticula::star_graph, nanobind::arg("size"));

  m.def("wheel_graph", &reticula::wheel_graph, nanobind::arg("size"));

  m.def("complete_graph", &reticula::complete_graph, nanobind::arg("size"));

  m.def(
    "complete_bipartite_graph", &reticula::complete_bipartite_graph,
    nanobind::arg("size_a"), nanobind::arg("size_b"));

  m.def(
    "complete_bipartite_directed_graph",
    &reticula::complete_bipartite_directed_graph, nanobind::arg("size_a"),
    nanobind::arg("size_b"));

  m.def(
    "complete_directed_graph", &reticula::complete_directed_graph,
    nanobind::arg("size"));

  m.def(
    "regular_ring_lattice", &reticula::regular_ring_lattice,
    nanobind::arg("size"), nanobind::arg("degree"));

  m.def(
    "square_grid_graph", &reticula::square_grid_graph,
    nanobind::arg("dimensions"), nanobind::arg("periodic") = false);
}
} // namespace reticula::python
