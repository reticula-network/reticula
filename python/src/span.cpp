#include <algorithm>
#include <cstddef>
#include <cstring>
#include <format>
#include <ranges>
#include <span>
#include <vector>

#include <reticula/edges.hpp>

#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/operators.h>

namespace reticula::python {
namespace {
template <typename T>
struct buffer_format;

template <>
struct buffer_format<std::int64_t> {
  static constexpr const char* value = "q";
};
template <>
struct buffer_format<std::uint64_t> {
  static constexpr const char* value = "Q";
};
template <>
struct buffer_format<std::int32_t> {
  static constexpr const char* value = "i";
};
template <>
struct buffer_format<std::uint32_t> {
  static constexpr const char* value = "I";
};
template <>
struct buffer_format<std::int16_t> {
  static constexpr const char* value = "h";
};
template <>
struct buffer_format<std::uint16_t> {
  static constexpr const char* value = "H";
};
template <>
struct buffer_format<std::int8_t> {
  static constexpr const char* value = "b";
};
template <>
struct buffer_format<std::uint8_t> {
  static constexpr const char* value = "B";
};
template <>
struct buffer_format<double> {
  static constexpr const char* value = "d";
};
template <>
struct buffer_format<float> {
  static constexpr const char* value = "f";
};

template <typename T>
constexpr const char* buffer_format_v = buffer_format<T>::value;

// NOLINTBEGIN  //buffer protocol madness
template <typename Vec>
struct buffer_helper {
  using T = typename Vec::value_type;

  struct view_info {
    Py_ssize_t shape[1];
    Py_ssize_t strides[1];
  };

  static int getbuffer(PyObject* obj, Py_buffer* v, int flags) noexcept;

  static void releasebuffer(PyObject*, Py_buffer* v) noexcept;
};

template <typename Vec>
void buffer_helper<Vec>::releasebuffer(PyObject*, Py_buffer* v) noexcept {
  delete static_cast<view_info*>(v->internal);
}

template <typename Vec>
int buffer_helper<Vec>::getbuffer(
  PyObject* obj, Py_buffer* v, int flags) noexcept {
  auto& vec = nanobind::cast<Vec&>(nanobind::handle(obj));

  // Refuse writable requests explicitly
  if (flags & PyBUF_WRITABLE) {
    PyErr_SetString(PyExc_BufferError, "span is read-only");
    return -1;
  }

  std::memset(v, 0, sizeof(Py_buffer));

  v->obj = obj; // keep `vec` alive
  v->buf = const_cast<T*>(vec.empty() ? nullptr : vec.data());
  v->len = static_cast<Py_ssize_t>(vec.size() * sizeof(T));
  v->itemsize = sizeof(T);
  v->readonly = 1;
  v->ndim = 1;

  // Attach heap-allocated shape/stride only if the caller asked for it
  if (flags & PyBUF_STRIDES) {
    auto* info = new view_info;
    info->shape[0] = static_cast<Py_ssize_t>(vec.size());
    info->strides[0] = sizeof(T);
    v->shape = info->shape;
    v->strides = info->strides;
    v->internal = info;
  }

  if (flags & PyBUF_FORMAT)
    v->format = const_cast<char*>(buffer_format_v<T>);

  Py_INCREF(obj); // buffer view owns a ref
  return 0;
}
// NOLINTEND

size_t wrap(Py_ssize_t i, std::size_t n) {
  if (i < 0)
    i += static_cast<Py_ssize_t>(n);

  if (i < 0 || static_cast<std::size_t>(i) >= n)
    throw nanobind::index_error("index out of range");

  return static_cast<std::size_t>(i);
}

template <typename T, std::size_t Extent = std::dynamic_extent>
void define_span(nanobind::module_& m, std::string name) {
  using SpanT = std::span<const T, Extent>;

  std::string span_name = "span_" + name;
  if constexpr (Extent != std::dynamic_extent)
    span_name += "_" + std::to_string(Extent);

  // NOLINTBEGIN  //buffer protocol madness
  std::vector<PyType_Slot> slots;
  slots.reserve(3);
  if constexpr (std::integral<T> || std::floating_point<T>) {
    using Helper = buffer_helper<SpanT>;
    slots.push_back(
      {Py_bf_getbuffer, reinterpret_cast<void*>(+&Helper::getbuffer)});
    slots.push_back(
      {Py_bf_releasebuffer, reinterpret_cast<void*>(&Helper::releasebuffer)});
  }
  slots.push_back({0, nullptr});
  // NOLINTEND

  auto cls = nanobind::class_<SpanT>(
    m, span_name.c_str(), nanobind::type_slots(slots.data()));

  if constexpr (std::integral<T> || std::floating_point<T>)
    cls
      .def(
        "__dlpack__",
        [](SpanT& v, nanobind::kwargs) {
          return nanobind::ndarray<const T>(v.data(), {v.size()});
        },
        nanobind::rv_policy::reference_internal)
      .def("__dlpack_device__", [](const SpanT&) {
        return nanobind::make_tuple(nanobind::device::cpu::value, 0uz);
      });

  cls
    .def(
      "__repr__",
      [span_name](const SpanT& v) {
        return nanobind::str(
          std::format("{}([{} items])", span_name, v.size()).c_str());
      })
    .def(
      "__len__", [](const SpanT& v) { return v.size(); },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__bool__", [](const SpanT& v) { return !v.empty(); },
      "Check whether the vector is nonempty",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__iter__",
      [](SpanT& v) {
        return nanobind::make_iterator<
          nanobind::rv_policy::automatic_reference>(
          nanobind::type<SpanT>(), "Iterator", v.begin(), v.end());
      },
      nanobind::keep_alive<0, 1>())
    .def(
      "__getitem__",
      [](SpanT& v, Py_ssize_t i) -> T { return v[wrap(i, v.size())]; },
      nanobind::rv_policy::automatic_reference,
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__getitem__",
      [](const SpanT& v, const nanobind::slice& slice) -> std::vector<T> {
        auto [start, stop, step, length] = slice.compute(v.size());
        std::vector<T> seq;
        seq.reserve(length);

        for (size_t i = 0; i < length; ++i) {
          seq.push_back(v[static_cast<std::size_t>(start)]);
          start += step;
        }

        return seq;
      })
    .def(
      "__eq__",
      [](const SpanT& a, const SpanT& b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end());
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__eq__",
      [](const SpanT& a, const std::vector<T>& b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end());
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__contains__",
      [](const SpanT& v, const T& x) {
        return std::find(v.begin(), v.end(), x) != v.end();
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__contains__", // fallback for incompatible types
      [](const SpanT&, nanobind::fallback) { return false; },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "count",
      [](const SpanT& v, const T& x) {
        return std::count(v.begin(), v.end(), x);
      },
      "Return number of occurrences of `arg`.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__hash__",
      [](const SpanT&) {
        throw nanobind::type_error("unhashable type: 'span'");
      },
      "Spans are unhashable.");
}
} // namespace

void define_span_types(nanobind::module_& m) {
  define_span<std::uint64_t, 1>(m, "uint64");
  define_span<std::uint64_t>(m, "uint64");
  define_span<std::int64_t>(m, "int64");
  define_span<double>(m, "double");
  define_span<float>(m, "float");

  define_span<std::pair<double, double>>(m, "pair_double_double");
  define_span<std::pair<std::uint64_t, std::uint64_t>>(m, "pair_uint64_uint64");

  define_span<undirected_edge>(m, "undirected_edge");
  define_span<directed_edge>(m, "directed_edge");
  define_span<undirected_temporal_edge>(m, "undirected_temporal_edge");
  define_span<directed_temporal_edge>(m, "directed_temporal_edge");
  define_span<directed_delayed_temporal_edge>(
    m, "directed_delayed_temporal_edge");

  define_span<undirected_hyperedge>(m, "undirected_hyperedge");
  define_span<directed_hyperedge>(m, "directed_hyperedge");
  define_span<undirected_temporal_hyperedge>(
    m, "undirected_temporal_hyperedge");
  define_span<directed_temporal_hyperedge>(m, "directed_temporal_hyperedge");
  define_span<directed_delayed_temporal_hyperedge>(
    m, "directed_delayed_temporal_hyperedge");
}
} // namespace reticula::python
