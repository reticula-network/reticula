#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/optional.h>

#include <reticula/generation/degree_sequence.hpp>

namespace reticula::python {
void define_degree_sequence(nanobind::module_& m) {
  m.def(
    "degree_sequence_graph",
    &reticula::degree_sequence_graph<std::mt19937_64, std::vector<VertexType>>,
    nanobind::arg("degree_sequence"), nanobind::arg("random_state"));

  m.def(
    "try_degree_sequence_graph",
    &reticula::try_degree_sequence_graph<
      std::mt19937_64, std::vector<VertexType>>,
    nanobind::arg("degree_sequence"), nanobind::arg("random_state"),
    nanobind::arg("max_tries"));

  m.def(
    "directed_degree_sequence_graph",
    &reticula::directed_degree_sequence_graph<
      std::mt19937_64, std::vector<std::pair<VertexType, VertexType>>>,
    nanobind::arg("in_out_degree_pair_sequence"),
    nanobind::arg("random_state"));

  m.def(
    "try_directed_degree_sequence_graph",
    &reticula::try_directed_degree_sequence_graph<
      std::mt19937_64, std::vector<std::pair<VertexType, VertexType>>>,
    nanobind::arg("in_out_degree_pair_sequence"), nanobind::arg("random_state"),
    nanobind::arg("max_tries"));
}
} // namespace reticula::python
