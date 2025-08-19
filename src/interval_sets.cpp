#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <reticula/concepts.hpp>
#include <reticula/interval_sets.hpp>

namespace reticula {
interval_set::interval_set() : ints_{}, total_{} {}

void interval_set::insert(double start, double end) {
  if (std::isnan(start) || std::isnan(end))
    throw std::invalid_argument("start and end cannot be NaN");

  if (end < start)
    throw std::invalid_argument("interval end cannot be less than the start");

  if (end == start)
    return;

  auto it = std::ranges::upper_bound(
    ints_, start, std::ranges::less{},
    [](const Interval& p) { return p.first; });

  if (it != ints_.begin()) {
    auto prev = std::prev(it);
    if (prev->second >= start) {
      start = std::min(start, prev->first);
      end = std::max(end, prev->second);
      it = prev;
    }
  }
  auto jt = it;
  while (jt != ints_.end() && jt->first <= end) {
    start = std::min(start, jt->first);
    end = std::max(end, jt->second);
    total_ -= (jt->second - jt->first);
    ++jt;
  }

  if (it == jt) {
    ints_.insert(it, {start, end});
  } else {
    *it = {start, end};
    ints_.erase(std::next(it), jt);
  }
  total_ += (end - start);
}

void interval_set::merge(const interval_set& other) {
  IntervalVector out;
  out.reserve((ints_.size() + other.ints_.size()));

  size_t i = 0, j = 0;
  auto push = [&](Interval seg) {
    if (out.empty()) {
      out.push_back(seg);
    } else if (seg.first <= out.back().second) {
      if (seg.second > out.back().second)
        out.back().second = seg.second;
    } else {
      out.push_back(seg);
    }
  };

  while (i < ints_.size() && j < other.ints_.size()) {
    if (ints_[i].first <= other.ints_[j].first)
      push(ints_[i++]);
    else
      push(other.ints_[j++]);
  }
  while (i < ints_.size())
    push(ints_[i++]);
  while (j < other.ints_.size())
    push(other.ints_[j++]);

  double new_total = 0.0;
  for (const auto& [s, e] : out)
    new_total += (e - s);

  ints_.swap(out);
  total_ = new_total;
}

auto interval_set::covers(double time) const -> bool {
  auto it = std::ranges::upper_bound(
    ints_, time, std::ranges::less{},
    [](const Interval& p) { return p.first; });
  if (it == ints_.begin())
    return false;
  const auto& seg = *std::prev(it);
  return (time > seg.first) && (time <= seg.second);
}

auto interval_set::cover() const -> double { return total_; }

auto interval_set::begin() const -> IteratorType { return ints_.cbegin(); }

auto interval_set::end() const -> IteratorType { return ints_.cend(); }

interval_set_cluster_sketch::interval_set_cluster_sketch(
  double resolution, std::size_t seed)
    : ints_{true, seed}, resolution_{resolution},
      window_start_{std::numeric_limits<double>::infinity()},
      window_end_{-std::numeric_limits<double>::infinity()} {
  if (!(std::isfinite(resolution_) && resolution_ > 0.0))
    throw std::invalid_argument("resolution must be finite and > 0");
}

void interval_set_cluster_sketch::insert(
  VertexType v, double start, double end) {
  if (std::isnan(start) || std::isnan(end))
    throw std::invalid_argument("start/end cannot be NaN");

  if (end < start)
    throw std::invalid_argument("interval end cannot be less than the start");

  if (end == start)
    return;

  window_start_ = std::min(window_start_, start);
  window_end_ = std::max(window_end_, end);

  if (
    window_start_ == -std::numeric_limits<double>::infinity() ||
    window_end_ == std::numeric_limits<double>::infinity())
    return;

  const BucketId k0 = bucket_for_(start);
  const double e_adj =
    std::nextafter(end, -std::numeric_limits<double>::infinity());
  const BucketId k1 = bucket_for_(e_adj);

  if (k1 < k0)
    return;

  add_range_(v, k0, k1);
}

void interval_set_cluster_sketch::merge(
  const interval_set_cluster_sketch& other) {
  window_start_ = std::min(window_start_, other.window_start_);
  window_end_ = std::max(window_end_, other.window_end_);

  if (
    window_start_ == -std::numeric_limits<double>::infinity() ||
    window_end_ == std::numeric_limits<double>::infinity())
    return;

  if (resolution_ != other.resolution_)
    throw std::invalid_argument(
      "cannot merge sketches with different resolutions");
  ints_.merge(other.ints_);
}

auto interval_set_cluster_sketch::bucket_for_(double x) const -> BucketId {
  return static_cast<BucketId>(std::floor(x / resolution_));
}

void interval_set_cluster_sketch::add_range_(
  VertexType v, BucketId k0, BucketId k1) {
  for (BucketId k = k0; k <= k1; ++k)
    ints_.insert({v, k});
}

auto interval_set_cluster_sketch::cover_estimate() const -> double {
  if (
    window_start_ == -std::numeric_limits<double>::infinity() ||
    window_end_ == std::numeric_limits<double>::infinity())
    return std::numeric_limits<double>::infinity();

  return resolution_ * ints_.estimate();
}
} // namespace reticula
