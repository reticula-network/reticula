#include <nanobind/nanobind.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/randomisation/link_shufflings.hpp>

namespace reticula::python {
namespace {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
void define_link_shufflings_for_net(nanobind::module_& m) {
  m.def(
    "link_shuffling", &reticula::link_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{});

  m.def(
    "connected_link_shuffling", &reticula::connected_link_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{});

  m.def(
    "topology_constrained_link_shuffling",
    &reticula::topology_constrained_link_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{});

  m.def(
    "degree_constrained_link_shuffling",
    &reticula::degree_constrained_link_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{},
    nanobind::arg("iters") = 5);

  m.def(
    "connected_degree_constrained_link_shuffling",
    &reticula::connected_degree_constrained_link_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{},
    nanobind::arg("iters") = 5);
}
} // namespace

void define_link_shufflings(nanobind::module_& m) {
  define_link_shufflings_for_net<undirected_temporal_network, std::mt19937_64>(
    m);
  define_link_shufflings_for_net<directed_temporal_network, std::mt19937_64>(m);
  define_link_shufflings_for_net<
    directed_delayed_temporal_network, std::mt19937_64>(m);
}
} // namespace reticula::python
