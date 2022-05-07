#ifndef INCLUDE_RETICULA_TYPE_TRAITS_HPP_
#define INCLUDE_RETICULA_TYPE_TRAITS_HPP_

namespace reticula {
  template <typename EdgeT> struct is_instantaneous {
    static constexpr bool value = false;
  };

  template <typename EdgeT>
  inline constexpr bool
  is_instantaneous_v = is_instantaneous<EdgeT>::value;

  template <typename EdgeT> struct is_undirected {
    static constexpr bool value = false;
  };

  template <typename EdgeT>
  inline constexpr bool
  is_undirected_v = is_undirected<EdgeT>::value;

  template <typename EdgeT> struct is_dyadic {
    static constexpr bool value = false;
  };

  template <typename EdgeT>
  inline constexpr bool
  is_dyadic_v = is_dyadic<EdgeT>::value;
}  // namespace reticula

#endif  // INCLUDE_RETICULA_TYPE_TRAITS_HPP_
