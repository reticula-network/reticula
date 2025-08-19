#include <reticula/algorithms/properties.hpp>

#include <reticula/concepts.hpp>

namespace reticula {
auto window::duration() const noexcept -> TimeType { return end - start; }

auto window::duration_is_valid() const noexcept -> bool { return end >= start; }

auto window::contains(TimeType t) const noexcept -> bool {
  return t >= start && t <= end;
}

auto window::contains(const window& other) const noexcept -> bool {
  return start <= other.start && end >= other.end;
}

auto window::overlaps(const window& other) const noexcept -> bool {
  return start <= other.end && end >= other.start;
}

auto window::intersection_with(const window& other) const noexcept -> window {
  return {
    .start = std::max(start, other.start), .end = std::min(end, other.end)};
}

auto window::union_with(const window& other) const noexcept -> window {
  return {
    .start = std::min(start, other.start), .end = std::max(end, other.end)};
}

auto window::operator==(const window& other) const noexcept -> bool {
  return start == other.start && end == other.end;
}

} // namespace reticula
