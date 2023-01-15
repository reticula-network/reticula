#ifdef RETICULA_SHIM_RANGES

#include <ranges>
namespace reticula {
  namespace ranges = std::ranges;
  namespace views = std::ranges::views;
}  // namespace reticula

#else

#include <range/v3/all.hpp>
namespace reticula {
  namespace ranges = ::ranges::cpp20;
  namespace views = ::ranges::cpp20::views;
}  // namespace reticula

#endif
