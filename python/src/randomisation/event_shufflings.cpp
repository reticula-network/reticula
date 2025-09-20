#include <nanobind/nanobind.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/randomisation/event_shufflings.hpp>

namespace reticula::python {
namespace {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
void define_event_shufflings_for_net(nanobind::module_& m) {
  m.def(
    "instant_event_shuffling", &reticula::instant_event_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"));
}
} // namespace

void define_event_shufflings(nanobind::module_& m) {
  define_event_shufflings_for_net<undirected_temporal_network, std::mt19937_64>(
    m);
  define_event_shufflings_for_net<directed_temporal_network, std::mt19937_64>(
    m);
  define_event_shufflings_for_net<
    directed_delayed_temporal_network, std::mt19937_64>(m);
}
} // namespace reticula::python
