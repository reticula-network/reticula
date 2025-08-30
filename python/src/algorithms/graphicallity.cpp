#include <nanobind/nanobind.h>

#include <reticula/algorithms/graphicallity.hpp>

namespace reticula::python {
void define_graphicallity(nanobind::module_& m) {
  m.def(
    "is_graphic", &reticula::is_graphic<std::vector<VertexType>>,
    nanobind::arg("degree_sequence"));

  m.def(
    "is_digraphic",
    &reticula::is_digraphic<std::vector<std::pair<VertexType, VertexType>>>,
    nanobind::arg("in_out_degree_sequence"));
}
} // namespace reticula::python
