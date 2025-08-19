#pragma once

#include <utility>
#include <vector>

#include <reticula/concepts.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
class interval_set {
public:
  using Interval = std::pair<double, double>;
  using IntervalVector = std::vector<Interval>;
  using IteratorType = IntervalVector::const_iterator;

  interval_set();

  void insert(double start, double end);

  void merge(const interval_set& other);

  [[nodiscard]] auto covers(double time) const -> bool;

  [[nodiscard]] auto cover() const -> double;

  [[nodiscard]] auto begin() const -> IteratorType;
  [[nodiscard]] auto end() const -> IteratorType;

  auto operator==(const interval_set& other) const -> bool = default;

private:
  IntervalVector ints_;
  double total_;
};

class interval_set_cluster_sketch {
public:
  using BucketId = std::int64_t;
  using IntervalSketch =
    hll::hyperloglog<std::pair<VertexType, BucketId>, 13, 14>;

  interval_set_cluster_sketch(double resolution, std::size_t seed);

  void insert(VertexType v, double start, double end);

  void merge(const interval_set_cluster_sketch& other);

  [[nodiscard]] auto cover_estimate() const -> double;

private:
  IntervalSketch ints_;
  double resolution_;

  double window_start_;
  double window_end_;

  [[nodiscard]] auto bucket_for_(double x) const -> BucketId;

  void add_range_(VertexType v, BucketId k0, BucketId k1);
};
} // namespace reticula

#include <reticula/utils.hpp>

namespace hll {
template <>
struct hash<std::pair<
  reticula::VertexType, reticula::interval_set_cluster_sketch::BucketId>> {
  auto operator()(
    const std::pair<
      reticula::VertexType, reticula::interval_set_cluster_sketch::BucketId>& p,
    std::uint64_t seed) const -> std::uint64_t {
    const std::size_t hv = hll::hash<reticula::VertexType>{}(p.first, seed);
    const std::size_t hb =
      hll::hash<reticula::interval_set_cluster_sketch::BucketId>{}(
        p.second, seed);
    return hv ^
           (hb + reticula::utils::golden_ratio + (seed << 6) + (seed >> 2));
  }
};
} // namespace hll
