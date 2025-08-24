#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>

#include <reticula/generation/uniform_hypergraph.hpp>

namespace reticula::python {
void define_uniform_hypergraph(nanobind::module_& m) {
  m.def(
    "random_uniform_hypergraph",
    &reticula::random_uniform_hypergraph<std::mt19937_64>,
    nanobind::arg("size"), nanobind::arg("edge_degree"),
    nanobind::arg("edge_prob"), nanobind::arg("random_state"));

  m.def(
    "random_directed_uniform_hypergraph",
    &reticula::random_directed_uniform_hypergraph<std::mt19937_64>,
    nanobind::arg("size"), nanobind::arg("edge_in_degree"),
    nanobind::arg("edge_out_degree"), nanobind::arg("edge_prob"),
    nanobind::arg("random_state"));
}
} // namespace reticula::python
