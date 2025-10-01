#include <format>
#include <random>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <reticula/processes.hpp>

namespace reticula::python {
void define_processes(nanobind::module_& m) {
  nanobind::class_<power_law>(m, "power_law")
    .def(
      nanobind::init<double, double>(), nanobind::arg("exponent"),
      nanobind::arg("mean"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const power_law& pl) {
        return std::format(
          "reticula.power_law(exponent={}, mean={})", pl.exponent(), pl.mean());
      })
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
      "__repr__",
      [](const residual_power_law& rpl) {
        return std::format(
          "reticula.residual_power_law(exponent={}, mean={})", rpl.exponent(),
          rpl.mean());
      })
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
      "__repr__",
      [](const hawkes_univariate_exponential& hue) {
        return std::format(
          "reticula.hawkes_univariate_exponential(mu={}, alpha={}, theta={}, "
          "phi={})",
          hue.mu(), hue.alpha(), hue.theta(), hue.phi());
      })
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
      "__repr__",
      [](const delta_distribution& dd) {
        return std::format("reticula.delta_distribution(mean={})", dd.mean());
      })
    .def(
      "__call__",
      [](const delta_distribution& dd, std::mt19937_64& g) { return dd(g); },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mean", &delta_distribution::mean,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::exponential_distribution<double>>(
    m, "exponential_distribution")
    .def(
      nanobind::init<double>(), nanobind::arg("lmbda"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::exponential_distribution<double>& ed) {
        return std::format(
          "reticula.exponential_distribution(lmbda={})", ed.lambda());
      })
    .def(
      "__call__",
      [](std::exponential_distribution<double>& ed, std::mt19937_64& g) {
        return ed(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "lmbda", &std::exponential_distribution<double>::lambda,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::uniform_real_distribution<double>>(
    m, "uniform_real_distribution")
    .def(
      nanobind::init<double, double>(), nanobind::arg("a"), nanobind::arg("b"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::uniform_real_distribution<double>& urd) {
        return std::format(
          "reticula.uniform_real_distribution(a={}, b={})", urd.a(), urd.b());
      })
    .def(
      "__call__",
      [](std::uniform_real_distribution<double>& urd, std::mt19937_64& g) {
        return urd(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "a", &std::uniform_real_distribution<double>::a,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "b", &std::uniform_real_distribution<double>::b,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::lognormal_distribution<double>>(
    m, "lognormal_distribution")
    .def(
      nanobind::init<double, double>(), nanobind::arg("m"), nanobind::arg("s"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::lognormal_distribution<double>& lnd) {
        return std::format(
          "reticula.lognormal_distribution(m={}, s={})", lnd.m(), lnd.s());
      })
    .def(
      "__call__",
      [](std::lognormal_distribution<double>& lnd, std::mt19937_64& g) {
        return lnd(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "m", &std::lognormal_distribution<double>::m,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "s", &std::lognormal_distribution<double>::s,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::normal_distribution<double>>(
    m, "normal_distribution")
    .def(
      nanobind::init<double, double>(), nanobind::arg("mean"),
      nanobind::arg("stddev"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::normal_distribution<double>& nd) {
        return std::format(
          "reticula.normal_distribution(mean={}, stddev={})", nd.mean(),
          nd.stddev());
      })
    .def(
      "__call__",
      [](std::normal_distribution<double>& nd, std::mt19937_64& g) {
        return nd(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mean", &std::normal_distribution<double>::mean,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "stddev", &std::normal_distribution<double>::stddev,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::gamma_distribution<double>>(m, "gamma_distribution")
    .def(
      nanobind::init<double, double>(), nanobind::arg("alpha"),
      nanobind::arg("beta"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::gamma_distribution<double>& gd) {
        return std::format(
          "reticula.gamma_distribution(alpha={}, beta={})", gd.alpha(),
          gd.beta());
      })
    .def(
      "__call__",
      [](std::gamma_distribution<double>& gd, std::mt19937_64& g) {
        return gd(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "alpha", &std::gamma_distribution<double>::alpha,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "beta", &std::gamma_distribution<double>::beta,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::weibull_distribution<double>>(m, "weibull_distribution")
    .def(
      nanobind::init<double, double>(), nanobind::arg("a"), nanobind::arg("b"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::weibull_distribution<double>& wd) {
        return std::format(
          "reticula.weibull_distribution(a={}, b={})", wd.a(), wd.b());
      })
    .def(
      "__call__",
      [](std::weibull_distribution<double>& wd, std::mt19937_64& g) {
        return wd(g);
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "a", &std::weibull_distribution<double>::a,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "b", &std::weibull_distribution<double>::b,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  nanobind::class_<std::geometric_distribution<std::uint64_t>>(
    m, "geometric_distribution")
    .def(
      nanobind::init<double>(), nanobind::arg("p"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const std::geometric_distribution<std::uint64_t>& gd) {
        return std::format("reticula.geometric_distribution(p={})", gd.p());
      })
    .def(
      "__call__",
      [](std::geometric_distribution<std::uint64_t>& gd, std::mt19937_64& g) {
        return static_cast<double>(gd(g));
      },
      nanobind::arg("generator"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "p", &std::geometric_distribution<std::uint64_t>::p,
      nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
