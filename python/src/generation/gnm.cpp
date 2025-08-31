#include <nanobind/nanobind.h>

#include <reticula/generation/gnm.hpp>

namespace reticula::python {
void define_gnm(nanobind::module_& m) {
  m.def(
    "random_gnm_graph", &reticula::random_gnm_graph<std::mt19937_64>,
    nanobind::arg("n"), nanobind::arg("m"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "random_directed_gnm_graph",
    &reticula::random_directed_gnm_graph<std::mt19937_64>, nanobind::arg("n"),
    nanobind::arg("m"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
