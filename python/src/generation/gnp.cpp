#include <nanobind/nanobind.h>

#include <reticula/generation/gnp.hpp>

namespace reticula::python {
void define_gnp(nanobind::module_& m) {
  m.def(
    "random_gnp_graph", &reticula::random_gnp_graph<std::mt19937_64>,
    nanobind::arg("n"), nanobind::arg("p"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "random_directed_gnp_graph",
    &reticula::random_directed_gnp_graph<std::mt19937_64>, nanobind::arg("n"),
    nanobind::arg("p"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
