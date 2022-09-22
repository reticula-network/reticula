namespace reticula {
  template <std::ranges::forward_range AttrPairRange>
  requires is_pairlike_of<
      std::ranges::range_value_t<AttrPairRange>, double, double>
  double pearson_correlation_coefficient(AttrPairRange&& f) {
    double mean_x = 0.0, mean_y = 0.0;
    double length = static_cast<double>(f.size());
    for (auto& [x, y]: f) {
      mean_x += x/length;
      mean_y += y/length;
    }

    double sum_sq_x = 0.0, sum_sq_y = 0.0, sum_mul = 0.0;
    for (auto& [x, y]: f) {
      sum_mul += (x - mean_x)*(y - mean_y);
      sum_sq_x += (x - mean_x)*(x - mean_x);
      sum_sq_y += (y - mean_y)*(y - mean_y);
    }

    return sum_mul/(std::sqrt(sum_sq_x)*std::sqrt(sum_sq_y));
  }
}  // namespace reticula
