#include <algorithm>
#include <cstddef>
#include <cstring>
#include <format>
#include <vector>

#include <reticula/edges.hpp>
#include <reticula/components.hpp>

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
  [[maybe_unused]]
  static constexpr const char* value = "q";
};
template <>
struct buffer_format<std::uint64_t> {
  [[maybe_unused]]
  static constexpr const char* value = "Q";
};
template <>
struct buffer_format<std::int32_t> {
  [[maybe_unused]]
  static constexpr const char* value = "i";
};
template <>
struct buffer_format<std::uint32_t> {
  [[maybe_unused]]
  static constexpr const char* value = "I";
};
template <>
struct buffer_format<std::int16_t> {
  [[maybe_unused]]
  static constexpr const char* value = "h";
};
template <>
struct buffer_format<std::uint16_t> {
  [[maybe_unused]]
  static constexpr const char* value = "H";
};
template <>
struct buffer_format<std::int8_t> {
  [[maybe_unused]]
  static constexpr const char* value = "b";
};
template <>
struct buffer_format<std::uint8_t> {
  [[maybe_unused]]
  static constexpr const char* value = "B";
};
template <>
struct buffer_format<double> {
  [[maybe_unused]]
  static constexpr const char* value = "d";
};
template <>
struct buffer_format<float> {
  [[maybe_unused]]
  static constexpr const char* value = "f";
};

template <typename T>
constexpr const char* buffer_format_v = buffer_format<T>::value;

template <typename From, typename To, typename ArrayT>
  requires(std::integral<To> || std::floating_point<To>)
bool convert_ndarray(std::vector<To>* v, const ArrayT& arr) {
  if (arr.dtype() != nanobind::dtype<From>())
    return false;

  auto vw = arr.template view<From>();
  for (std::size_t i = 0; i < arr.size(); i++) {
    auto val = vw(i);
    if constexpr (std::integral<To> && std::integral<From>)
      if (!std::in_range<To>(val))
        throw std::overflow_error("value is outside representable range");
    v->push_back(static_cast<To>(val));
  }
  return true;
}

template <typename To>
void initialise_vector_from_ndarray(
  std::vector<To>* v,
  const nanobind::ndarray<
    nanobind::ndim<1>, nanobind::device::cpu, nanobind::any_contig>& arr) {
  new (v) std::vector<To>{};
  v->reserve(arr.size());

  auto success = convert_ndarray<std::uint64_t>(v, arr) ||
                 convert_ndarray<std::int64_t>(v, arr) ||
                 convert_ndarray<std::uint32_t>(v, arr) ||
                 convert_ndarray<std::int32_t>(v, arr) ||
                 convert_ndarray<std::uint16_t>(v, arr) ||
                 convert_ndarray<std::int16_t>(v, arr) ||
                 convert_ndarray<std::uint8_t>(v, arr) ||
                 convert_ndarray<std::int8_t>(v, arr);
  if constexpr (std::floating_point<To>)
    success = success || convert_ndarray<double>(v, arr) ||
              convert_ndarray<float>(v, arr);

  if (!success)
    throw nanobind::next_overload();
}

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

  std::memset(v, 0, sizeof(Py_buffer));

  v->obj = obj; // keep `vec` alive
  v->buf = vec.empty() ? nullptr : vec.data();
  v->len = static_cast<Py_ssize_t>(vec.size() * sizeof(T));
  v->itemsize = sizeof(T);
  v->readonly = 0;
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

template <typename T>
void define_vector(nanobind::module_& m, std::string name) {
  std::string vector_name = "vector_" + name;

  // NOLINTBEGIN  //buffer protocol madness
  std::vector<PyType_Slot> slots;
  slots.reserve(3);
  if constexpr (std::integral<T> || std::floating_point<T>) {
    using Helper = buffer_helper<std::vector<T>>;
    slots.push_back(
      {Py_bf_getbuffer, reinterpret_cast<void*>(+&Helper::getbuffer)});
    slots.push_back(
      {Py_bf_releasebuffer, reinterpret_cast<void*>(&Helper::releasebuffer)});
  }
  slots.push_back({0, nullptr});
  // NOLINTEND

  auto cls = nanobind::class_<std::vector<T>>(
    m, vector_name.c_str(), nanobind::type_slots(slots.data()));

  if constexpr (std::integral<T> || std::floating_point<T>)
    cls
      .def(
        "__init__",
        [](
          std::vector<T>* v, const nanobind::ndarray<
                               T, nanobind::ndim<1>, nanobind::device::cpu,
                               nanobind::any_contig>& arr) {
          std::span<const T> s{arr.data(), arr.size()};
          new (v) std::vector<T>{s.begin(), s.end()};
        },
        nanobind::call_guard<nanobind::gil_scoped_release>{})

      .def(
        "__init__", &initialise_vector_from_ndarray<T>,
        nanobind::call_guard<nanobind::gil_scoped_release>{})
      .def(
        "__dlpack__",
        [](std::vector<T>& v, nanobind::kwargs) {
          return nanobind::ndarray<T>(v.data(), {v.size()});
        },
        nanobind::rv_policy::reference_internal)
      .def("__dlpack_device__", [](const std::vector<T>&) {
        return nanobind::make_tuple(nanobind::device::cpu::value, 0uz);
      });

  cls
    .def(
      "__init__",
      [](std::vector<T>* v, nanobind::typed<nanobind::list, T> seq) {
        new (v) std::vector<T>{};
        auto size = static_cast<std::size_t>(PyList_Size(seq.ptr()));
        v->reserve(size);
        for (std::size_t i = 0; i < size; i++) {
          nanobind::handle h(
            PyList_GetItem(seq.ptr(), static_cast<Py_ssize_t>(i)));
          v->push_back(nanobind::cast<T>(h, true));
        }
      },
      "Construct from a list")
    .def(
      "__init__",
      [](std::vector<T>* v, nanobind::typed<nanobind::iterable, T> seq) {
        new (v) std::vector<T>{};
        v->reserve(nanobind::len_hint(seq));
        for (nanobind::handle h : seq)
          v->push_back(nanobind::cast<T>(h, true));
      },
      "Construct from an iterable object")
    .def(
      "__repr__",
      [vector_name](const std::vector<T>& v) {
        return nanobind::str(
          std::format("{}([{} items])", vector_name, v.size()).c_str());
      })
    .def(
      nanobind::init<>(), "Default constructor",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      nanobind::init<const std::vector<T>&>(), "Copy constructor",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__copy__",
      [](const std::vector<T>& self) { return std::vector<T>(self); },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__len__", [](const std::vector<T>& v) { return v.size(); },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__bool__", [](const std::vector<T>& v) { return !v.empty(); },
      "Check whether the vector is nonempty",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__iter__",
      [](std::vector<T>& v) {
        return nanobind::make_iterator<
          nanobind::rv_policy::automatic_reference>(
          nanobind::type<std::vector<T>>(), "Iterator", v.begin(), v.end());
      },
      nanobind::keep_alive<0, 1>())
    .def(
      "__getitem__",
      [](std::vector<T>& v, Py_ssize_t i) -> T { return v[wrap(i, v.size())]; },
      nanobind::rv_policy::automatic_reference,
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "clear", [](std::vector<T>& v) { v.clear(); },
      "Remove all items from list.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "append", [](std::vector<T>& v, const T& value) { v.push_back(value); },
      "Append `arg` to the end of the list.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "insert",
      [](std::vector<T>& v, Py_ssize_t i, const T& x) {
        if (i < 0)
          i += static_cast<Py_ssize_t>(v.size());
        if (i < 0 || static_cast<size_t>(i) > v.size())
          throw nanobind::index_error("index out of range");
        v.insert(v.begin() + i, x);
      },
      "Insert object `arg1` before index `arg0`.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "pop",
      [](std::vector<T>& v, Py_ssize_t i) {
        size_t index = wrap(i, v.size());
        T result = std::move(v[index]);
        v.erase(v.begin() + static_cast<std::ptrdiff_t>(index));
        return result;
      },
      nanobind::arg("index") = -1,
      "Remove and return item at `index` (default last).",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "extend",
      [](std::vector<T>& v, const std::vector<T>& src) {
        v.insert(v.end(), src.begin(), src.end());
      },
      "Extend `self` by appending elements from `arg`.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__setitem__",
      [](std::vector<T>& v, Py_ssize_t i, const T& value) {
        v[wrap(i, v.size())] = value;
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__delitem__",
      [](std::vector<T>& v, Py_ssize_t i) {
        v.erase(v.begin() + static_cast<std::ptrdiff_t>(wrap(i, v.size())));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__getitem__",
      [](const std::vector<T>& v, const nanobind::slice& slice)
        -> std::vector<T> {
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
      "__setitem__",
      [](
        std::vector<T>& v, const nanobind::slice& slice,
        const std::vector<T>& value) {
        auto [start, stop, step, length] = slice.compute(v.size());

        if (length != value.size())
          throw nanobind::index_error(
            "The left and right hand side of the slice "
            "assignment have mismatched sizes!");

        for (size_t i = 0; i < length; ++i) {
          v[static_cast<std::size_t>(start)] = value[i];
          start += step;
        }
      })
    .def(
      "__delitem__",
      [](std::vector<T>& v, const nanobind::slice& slice) {
        auto [start, stop, step, length] = slice.compute(v.size());
        if (length == 0)
          return;

        stop = start + (static_cast<Py_ssize_t>(length) - 1) * step;
        if (start > stop) {
          std::swap(start, stop);
          step = -step;
        }

        if (step == 1) {
          v.erase(v.begin() + start, v.begin() + stop + 1);
        } else {
          for (size_t i = 0; i < length; ++i) {
            v.erase(v.begin() + stop);
            stop -= step;
          }
        }
      })
    .def(
      "__eq__",
      [](const std::vector<T>& a, const std::vector<T>& b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end());
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__eq__",
      [](const std::vector<T>& a, const std::span<const T>& b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end());
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__contains__",
      [](const std::vector<T>& v, const T& x) {
        return std::find(v.begin(), v.end(), x) != v.end();
      },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__contains__", // fallback for incompatible types
      [](const std::vector<T>&, nanobind::fallback) { return false; },
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "count",
      [](const std::vector<T>& v, const T& x) {
        return std::count(v.begin(), v.end(), x);
      },
      "Return number of occurrences of `arg`.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "remove",
      [](std::vector<T>& v, const T& x) {
        auto p = std::find(v.begin(), v.end(), x);
        if (p != v.end())
          v.erase(p);
        else
          throw nanobind::value_error();
      },
      "Remove first occurrence of `arg`.",
      nanobind::call_guard<nanobind::gil_scoped_release>{})
    .def(
      "__hash__",
      [](const std::vector<T>&) {
        throw nanobind::type_error("unhashable type: 'vector'");
      },
      "Vectors are unhashable.");

  nanobind::implicitly_convertible<
    nanobind::typed<nanobind::list, T>, std::vector<T>>();
  nanobind::implicitly_convertible<
    nanobind::typed<nanobind::iterable, T>, std::vector<T>>();
}
} // namespace

void define_vector_types(nanobind::module_& m) {
  define_vector<std::uint64_t>(m, "uint64");
  define_vector<std::int64_t>(m, "int64");
  define_vector<std::uint32_t>(m, "uint32");
  define_vector<std::int32_t>(m, "int32");
  define_vector<double>(m, "double");
  define_vector<float>(m, "float");

  define_vector<std::pair<double, double>>(m, "pair_double_double");
  define_vector<std::pair<std::uint64_t, std::uint64_t>>(
    m, "pair_uint64_uint64");

  define_vector<undirected_edge>(m, "undirected_edge");
  define_vector<directed_edge>(m, "directed_edge");
  define_vector<undirected_temporal_edge>(m, "undirected_temporal_edge");
  define_vector<directed_temporal_edge>(m, "directed_temporal_edge");
  define_vector<directed_delayed_temporal_edge>(
    m, "directed_delayed_temporal_edge");

  define_vector<undirected_hyperedge>(m, "undirected_hyperedge");
  define_vector<directed_hyperedge>(m, "directed_hyperedge");
  define_vector<undirected_temporal_hyperedge>(
    m, "undirected_temporal_hyperedge");
  define_vector<directed_temporal_hyperedge>(m, "directed_temporal_hyperedge");
  define_vector<directed_delayed_temporal_hyperedge>(
    m, "directed_delayed_temporal_hyperedge");

  define_vector<component>(m, "component");
  define_vector<std::pair<VertexType, component>>(m, "pair_uint64_component");
  define_vector<std::pair<VertexType, double>>(m, "pair_uint64_double");

  if constexpr (!std::same_as<std::size_t, std::uint64_t>)
    define_vector<std::size_t>(m, "sizet");
}
} // namespace reticula::python
