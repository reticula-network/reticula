#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>

#include <reticula/algorithms/dag.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {


template <static_network_like NetT>
requires is_directed_v<NetT>
void define_dag_for_net(nanobind::module_& m) {
  m.def(
    "is_acyclic", reticula::is_acyclic<NetT>, nanobind::arg("net"));
  m.def(
    "try_topological_order", reticula::try_topological_order<NetT>,
    nanobind::arg("net"));
  m.def(
    "topological_order", reticula::topological_order<NetT>,
    nanobind::arg("net"));
}
} // namespace

void define_dag(nanobind::module_& m) {
  define_dag_for_net<directed_network>(m);
  define_dag_for_net<directed_hypernetwork>(m);
}
} // namespace reticula::python
