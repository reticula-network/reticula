#ifndef INCLUDE_RETICULA_RANGES_HPP_
#define INCLUDE_RETICULA_RANGES_HPP_

#ifdef RETICULA_SHIM_RANGES

#include <range/v3/all.hpp>
namespace reticula {
  namespace ranges = ::ranges::cpp20;
  namespace views = ::ranges::cpp20::views;
}  // namespace reticula

#else

#include <ranges>
namespace reticula {
  namespace ranges = std::ranges;
  namespace views = std::ranges::views;
}  // namespace reticula

#endif  // RETICULA_SHIM_RANGES

#endif  // INCLUDE_RETICULA_RANGES_HPP_
