// IWYU pragma: always_keep
#pragma once

#include <format>

#include <catch2/catch_tostring.hpp>

namespace Catch {
template <std::formattable<char> T>
struct StringMaker<T> {
  static auto convert(T const& v) -> std::string { return std::format("{}", v); }
};
} // namespace Catch
