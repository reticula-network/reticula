#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>

#include <reticula/generation/regular.hpp>

namespace reticula::python {
void define_regular(nanobind::module_& m) {
  m.def(
    "try_random_regular_graph",
    &reticula::try_random_regular_graph<std::mt19937_64>, nanobind::arg("size"),
    nanobind::arg("degree"), nanobind::arg("random_state"),
    nanobind::arg("max_tries"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "random_regular_graph", &reticula::random_regular_graph<std::mt19937_64>,
    nanobind::arg("size"), nanobind::arg("degree"),
    nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "try_random_directed_regular_graph",
    &reticula::try_random_directed_regular_graph<std::mt19937_64>,
    nanobind::arg("size"), nanobind::arg("degree"),
    nanobind::arg("random_state"), nanobind::arg("max_tries"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "random_directed_regular_graph",
    &reticula::random_directed_regular_graph<std::mt19937_64>,
    nanobind::arg("size"), nanobind::arg("degree"),
    nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
