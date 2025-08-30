#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/unordered_map.h>

#include <reticula/algorithms/assortativity.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {

template <network_like NetT>
void define_assortativity_for_net(nanobind::module_& m) {
  if constexpr (is_undirected_v<NetT>) {
    m.def(
      "attribute_assortativity",
      reticula::attribute_assortativity<
        NetT, std::function<double(VertexType)>>,
      nanobind::arg("net"), nanobind::arg("attr_fun"));

    m.def(
      "attribute_assortativity",
      reticula::attribute_assortativity<
        NetT, std::unordered_map<VertexType, double>>,
      nanobind::arg("net"), nanobind::arg("attr_map"),
      nanobind::arg("default_value"));

    m.def(
      "degree_assortativity",
      nanobind::overload_cast<const NetT&>(
        &reticula::degree_assortativity<NetT>),
      nanobind::arg("net"));
  }

  m.def(
    "attribute_assortativity",
    reticula::attribute_assortativity<
      NetT, std::function<double(VertexType)>,
      std::function<double(VertexType)>>,
    nanobind::arg("net"), nanobind::arg("mutator_attr_fun"),
    nanobind::arg("mutated_attr_fun"));

  m.def(
    "attribute_assortativity",
    reticula::attribute_assortativity<
      NetT, std::unordered_map<VertexType, double>,
      std::unordered_map<VertexType, double>>,
    nanobind::arg("net"), nanobind::arg("mutator_attr_map"),
    nanobind::arg("mutated_attr_map"), nanobind::arg("mutator_default_value"),
    nanobind::arg("mutated_default_value"));

  m.def(
    "degree_assortativity",
    nanobind::overload_cast<const NetT&, direction, direction>(
      &reticula::degree_assortativity<NetT>),
    nanobind::arg("net"), nanobind::arg("tail"), nanobind::arg("head"));
}
} // namespace

void define_assortativity(nanobind::module_& m) {
  define_assortativity_for_net<undirected_network>(m);
  define_assortativity_for_net<directed_network>(m);
  define_assortativity_for_net<undirected_hypernetwork>(m);
  define_assortativity_for_net<directed_hypernetwork>(m);

  nanobind::enum_<reticula::direction>(m, "direction")
    .value("in_deg", reticula::direction::in)
    .value("out_deg", reticula::direction::out);
}
} // namespace reticula::python
