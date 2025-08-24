#include <nanobind/nanobind.h>

#include <reticula/generation/barabasi_albert.hpp>

namespace reticula::python {
void define_barabasi_albert(nanobind::module_& m) {
  m.def(
    "random_barabasi_albert_graph",
    &reticula::random_barabasi_albert_graph<std::mt19937_64>,
    nanobind::arg("n"), nanobind::arg("m"), nanobind::arg("random_state"));
}
}
