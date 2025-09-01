#include <functional>
#include <random>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/variant.h>

#include <reticula/concepts.hpp>
#include <reticula/generation/node_activation.hpp>
#include <reticula/processes.hpp>

namespace reticula::python {
namespace {
template <static_network_edge EdgeT>
void define_node_activation_for_edge(nanobind::module_& m) {
  using dist_variant = std::variant<
    power_law, residual_power_law, hawkes_univariate_exponential,
    std::exponential_distribution<double>,
    std::uniform_real_distribution<double>, std::normal_distribution<double>,
    std::lognormal_distribution<double>, std::gamma_distribution<double>,
    std::weibull_distribution<double>,
    std::geometric_distribution<std::uint64_t>,
    std::function<double(std::mt19937_64&)>>;

  m.def(
    "random_node_activation_temporal_network",
    [](
      const network<EdgeT>& base_network, double max_t, dist_variant iet_dist,
      dist_variant res_dist, std::mt19937_64& random_state,
      std::size_t size_hint) {
      return std::visit(
        [&](auto&& iet, auto&& res) {
          return reticula::random_node_activation_temporal_network<
            EdgeT, decltype(iet), decltype(res), std::mt19937_64>(
            base_network, max_t, iet, res, random_state, size_hint);
        },
        iet_dist, res_dist);
    },
    nanobind::arg("base_network"), nanobind::arg("max_t"),
    nanobind::arg("iet_dist"), nanobind::arg("res_dist"),
    nanobind::arg("random_state"), nanobind::arg("size_hint") = 0,
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "random_node_activation_temporal_network_with_burn_in",
    [](
      const network<EdgeT>& base_network, double max_t, dist_variant iet_dist,
      std::mt19937_64& random_state, std::size_t size_hint) {
      return std::visit(
        [&](auto&& dist) {
          return reticula::random_node_activation_temporal_network_with_burn_in<
            EdgeT, decltype(dist), std::mt19937_64>(
            base_network, max_t, dist, random_state, size_hint);
        },
        iet_dist);
    },
    nanobind::arg("base_network"), nanobind::arg("max_t"),
    nanobind::arg("iet_dist"), nanobind::arg("random_state"),
    nanobind::arg("size_hint") = 0,
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_node_activation(nanobind::module_& m) {
  define_node_activation_for_edge<undirected_edge>(m);
  define_node_activation_for_edge<directed_edge>(m);
  define_node_activation_for_edge<undirected_hyperedge>(m);
  define_node_activation_for_edge<directed_hyperedge>(m);
}
} // namespace reticula::python
