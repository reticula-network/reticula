#ifndef INCLUDE_RETICULA_STATS_HPP_
#define INCLUDE_RETICULA_STATS_HPP_

#include <cmath>
#include <ranges>

#include "network_concepts.hpp"

namespace reticula {
  /**
    Calculates Pearson's correlation coeafficient of the two variables in the
    vector f.
  */
  template <std::ranges::forward_range AttrPairRange>
  requires is_pairlike_of<
      std::ranges::range_value_t<AttrPairRange>, double, double>
  double pearson_correlation_coefficient(AttrPairRange&& f);
}  // namespace reticula


#include "../../src/stats.tpp"

#endif  // INCLUDE_RETICULA_STATS_HPP_
