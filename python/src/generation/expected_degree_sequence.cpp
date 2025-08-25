#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>

#include <random>
#include <reticula/generation/expected_degree_sequence.hpp>

namespace reticula::python {
void define_expected_degree_sequence(nanobind::module_& m) {
  m.def(
    "expected_degree_sequence_graph",
    &reticula::expected_degree_sequence_graph<
      std::mt19937_64, std::vector<double>>,
    nanobind::arg("weight_sequence"), nanobind::arg("random_state"),
    nanobind::arg("self_loops") = false);

  m.def(
    "directed_expected_degree_sequence_graph",
    &reticula::directed_expected_degree_sequence_graph<
      std::mt19937_64, std::vector<std::pair<double, double>>>,
    nanobind::arg("in_out_weight_sequence"), nanobind::arg("random_state"),
    nanobind::arg("self_loops") = false);

  m.def(
    "expected_degree_sequence_hypergraph",
    &reticula::expected_degree_sequence_hypergraph<
      std::mt19937_64, std::vector<double>, std::vector<double>>,
    nanobind::arg("vertex_weight_sequence"),
    nanobind::arg("edge_weight_sequence"), nanobind::arg("random_state"));

  m.def(
    "directed_expected_degree_sequence_hypergraph",
    &reticula::directed_expected_degree_sequence_hypergraph<
      std::mt19937_64, std::vector<std::pair<double, double>>,
      std::vector<std::pair<double, double>>>,
    nanobind::arg("vertex_in_out_weight_sequence"),
    nanobind::arg("edge_in_out_weight_sequence"),
    nanobind::arg("random_state"));
}
} // namespace reticula::python
