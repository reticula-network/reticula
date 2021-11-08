#ifndef INCLUDE_DAG_TYPE_TRAITS_HPP_
#define INCLUDE_DAG_TYPE_TRAITS_HPP_

#include "network_concepts.hpp"

namespace dag {
  template <network_edge EdgeT> struct is_instantaneous {
    static constexpr bool value = false;
  };

  template <network_edge EdgeT>
  inline constexpr bool
  is_instantaneous_v = is_instantaneous<EdgeT>::value;

  template <network_edge EdgeT> struct is_undirected {
    static constexpr bool value = false;
  };

  template <network_edge EdgeT>
  inline constexpr bool
  is_undirected_v = is_undirected<EdgeT>::value;
}  // namespace dag

#endif  // INCLUDE_DAG_TYPE_TRAITS_HPP_
