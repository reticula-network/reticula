#include <random>
#include <functional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>

#include <reticula/concepts.hpp>
#include <reticula/generation/link_activation.hpp>

namespace reticula::python {
namespace {
template <static_network_edge EdgeT>
void define_link_activation_for_edge(nanobind::module_& m) {
  using dist_type = std::function<double(std::mt19937_64&)>;

  m.def(
    "random_link_activation_temporal_network",
    &reticula::random_link_activation_temporal_network<
      EdgeT, dist_type, dist_type, std::mt19937_64>,
    nanobind::arg("base_network"), nanobind::arg("max_t"),
    nanobind::arg("iet_dist"), nanobind::arg("res_dist"),
    nanobind::arg("random_state"), nanobind::arg("size_hint") = 0);

  m.def(
    "random_link_activation_temporal_network",
    &reticula::random_link_activation_temporal_network<
      EdgeT, dist_type, std::mt19937_64>,
    nanobind::arg("base_network"), nanobind::arg("max_t"),
    nanobind::arg("iet_dist"), nanobind::arg("random_state"),
    nanobind::arg("size_hint") = 0);
}
}

void define_link_activation(nanobind::module_& m) {
  define_link_activation_for_edge<undirected_edge>(m);
  define_link_activation_for_edge<directed_edge>(m);
  define_link_activation_for_edge<undirected_hyperedge>(m);
  define_link_activation_for_edge<directed_hyperedge>(m);
}
} // namespace reticula::python
