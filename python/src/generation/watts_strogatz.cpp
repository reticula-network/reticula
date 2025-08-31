#include <nanobind/nanobind.h>

#include <reticula/generation/watts_strogatz.hpp>

namespace reticula::python {
void define_watts_strogatz(nanobind::module_& m) {
  m.def(
    "random_watts_strogatz_graph",
    &reticula::random_watts_strogatz_graph<std::mt19937_64>, nanobind::arg("n"),
    nanobind::arg("k"), nanobind::arg("beta"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
