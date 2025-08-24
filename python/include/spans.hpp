#pragma once

#include <ranges>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

namespace reticula::python::utils {
template <auto M>
struct ndarray_output_helper;

template <class C, class R, class... Args, R (C::*M)(Args...)>
struct ndarray_output_helper<M> {
  static auto make() {
    return [](C& self, Args... args) {
      decltype(auto) rng = (self.*M)(std::forward<Args>(args)...);
      using Range = std::remove_reference_t<decltype(rng)>;
      static_assert(
        std::ranges::contiguous_range<Range>, "Result must be contiguous");
      static_assert(std::ranges::sized_range<Range>, "Result must be sized");
      using Elem =
        std::remove_reference_t<std::ranges::range_reference_t<Range>>;
      if constexpr (std::ranges::borrowed_range<Range>)
        return nanobind::ndarray<Elem, nanobind::numpy>(
          std::data(rng), {std::ranges::size(rng)});
      else
        return nanobind::ndarray<Elem, nanobind::numpy>(
                 std::data(rng), {std::ranges::size(rng)})
          .cast();
    };
  }
};

template <class C, class R, class... Args, R (C::*M)(Args...) const>
struct ndarray_output_helper<M> {
  static auto make() {
    return [](const C& self, Args... args) {
      decltype(auto) rng = (self.*M)(std::forward<Args>(args)...);
      using Range = std::remove_reference_t<decltype(rng)>;
      static_assert(
        std::ranges::contiguous_range<Range>, "Result must be contiguous");
      static_assert(std::ranges::sized_range<Range>, "Result must be sized");
      using Elem =
        std::remove_reference_t<std::ranges::range_reference_t<Range>>;
      if constexpr (std::ranges::borrowed_range<Range>)
        return nanobind::ndarray<Elem, nanobind::numpy>(
          std::data(rng), {std::ranges::size(rng)});
      else
        return nanobind::ndarray<Elem, nanobind::numpy>(
                 std::data(rng), {std::ranges::size(rng)})
          .cast();
    };
  }
};

template <class R, class C, class... Args, R (*M)(C&, Args...)>
struct ndarray_output_helper<M> {
  static auto make() {
    return [](C& self, Args... args) {
      decltype(auto) rng = M(self, std::forward<Args>(args)...);
      using Range = std::remove_reference_t<decltype(rng)>;
      static_assert(
        std::ranges::contiguous_range<Range>, "Result must be contiguous");
      static_assert(std::ranges::sized_range<Range>, "Result must be sized");
      using Elem =
        std::remove_reference_t<std::ranges::range_reference_t<Range>>;
      if constexpr (std::ranges::borrowed_range<Range>)
        return nanobind::ndarray<Elem, nanobind::numpy>(
          std::data(rng), {std::ranges::size(rng)});
      else
        return nanobind::ndarray<Elem, nanobind::numpy>(
                 std::data(rng), {std::ranges::size(rng)})
          .cast();
    };
  }
};

template <auto M>
auto ndarray_output() {
  return ndarray_output_helper<M>::make();
}
} // namespace reticula::python::utils
