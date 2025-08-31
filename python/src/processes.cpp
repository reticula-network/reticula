#include <nanobind/nanobind.h>

#include <reticula/processes.hpp>

namespace reticula::python {
void define_processes(nanobind::module_& m) {
  nanobind::class_<power_law>(m, "power_law")
    .def(
      nanobind::init<double, double>(), nanobind::arg("exponent"),
      nanobind::arg("mean"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__call__", [](const power_law& pl, std::mt19937_64& g) { return pl(g); },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "x_min", &power_law::x_min,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "exponent", &power_law::exponent,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mean", &power_law::mean,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<residual_power_law>(m, "residual_power_law")
    .def(
      nanobind::init<double, double>(), nanobind::arg("exponent"),
      nanobind::arg("mean"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__call__",
      [](const residual_power_law& rpl, std::mt19937_64& g) { return rpl(g); },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "x_min", &residual_power_law::x_min,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "exponent", &residual_power_law::exponent,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mean", &residual_power_law::mean,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<hawkes_univariate_exponential>(
    m, "hawkes_univariate_exponential")
    .def(
      nanobind::init<double, double, double, double>(), nanobind::arg("mu"),
      nanobind::arg("alpha"), nanobind::arg("theta"),
      nanobind::arg("phi") = 0.0,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__call__",
      [](hawkes_univariate_exponential& hue, std::mt19937_64& g) {
        return hue(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mu", &hawkes_univariate_exponential::mu,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "alpha", &hawkes_univariate_exponential::alpha,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "theta", &hawkes_univariate_exponential::theta,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "phi", &hawkes_univariate_exponential::phi,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<delta_distribution>(m, "delta_distribution")
    .def(
      nanobind::init<double>(), nanobind::arg("mean"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__call__",
      [](const delta_distribution& dd, std::mt19937_64& g) { return dd(g); },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mean", &delta_distribution::mean,
      nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
