#include <random>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/unordered_map.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/operations/occupation.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_occupation_for_net(nanobind::module_& m) {
  m.def(
    "uniformly_occupy_vertices",
    reticula::uniformly_occupy_vertices<NetT, std::mt19937_64>,
    nanobind::arg("g"), nanobind::arg("p"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
  m.def(
    "occupy_vertices",
    reticula::occupy_vertices<
      NetT, std::function<double(VertexType)>, std::mt19937_64>,
    nanobind::arg("g"), nanobind::arg("prob_func"),
    nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
  m.def(
    "occupy_vertices",
    reticula::occupy_vertices<
      NetT, std::unordered_map<VertexType, double>, std::mt19937_64>,
    nanobind::arg("g"), nanobind::arg("prob_map"),
    nanobind::arg("random_state"), nanobind::arg("default_prob") = 0.0,
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "uniformly_occupy_edges",
    reticula::uniformly_occupy_edges<NetT, std::mt19937_64>, nanobind::arg("g"),
    nanobind::arg("p"), nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
  m.def(
    "occupy_edges",
    reticula::occupy_edges<
      NetT, std::function<double(typename NetT::EdgeType)>, std::mt19937_64>,
    nanobind::arg("g"), nanobind::arg("prob_func"),
    nanobind::arg("random_state"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
  m.def(
    "occupy_edges",
    reticula::occupy_edges<
      NetT, std::unordered_map<typename NetT::EdgeType, double>,
      std::mt19937_64>,
    nanobind::arg("g"), nanobind::arg("prob_map"),
    nanobind::arg("random_state"), nanobind::arg("default_prob") = 0.0,
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_occupation(nanobind::module_& m) {
  define_occupation_for_net<undirected_network>(m);
  define_occupation_for_net<directed_network>(m);
  define_occupation_for_net<undirected_temporal_network>(m);
  define_occupation_for_net<directed_temporal_network>(m);
  define_occupation_for_net<directed_delayed_temporal_network>(m);

  define_occupation_for_net<undirected_hypernetwork>(m);
  define_occupation_for_net<directed_hypernetwork>(m);
  define_occupation_for_net<undirected_temporal_hypernetwork>(m);
  define_occupation_for_net<directed_temporal_hypernetwork>(m);
  define_occupation_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
