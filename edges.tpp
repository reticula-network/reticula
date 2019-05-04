#define GOLDEN_RATIO 0x9E3779B97F4A7C15

template <typename T>
inline size_t combine_hash(const size_t seed, const T& other) {
  return seed ^ (std::hash<T>{}(other) + GOLDEN_RATIO + (seed<<6) + (seed>>2));
}

template <typename T1, typename T2>
inline size_t unordered_hash(const T1& t1, const T2& t2) {
  size_t h1, h2;
  std::tie(h1, h2) = std::minmax(std::hash<T1>{}(t1), std::hash<T2>{}(t2));
  return h1 ^ (h2 + GOLDEN_RATIO + (h1<<6) + (h1>>2));
}

namespace std {
  template<typename VertT, typename TimeT>
  struct hash<dag::undirected_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::undirected_temporal_edge<VertT, TimeT>& e) const {
      return combine_hash(unordered_hash(e.v1, e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::directed_temporal_edge<VertT, TimeT>& e) const {
      return combine_hash(combine_hash(std::hash<VertT>{}(e.v1), e.v2), e.time);
    }
  };

  template<typename VertT>
  struct hash<dag::directed_edge<VertT>> {
    size_t
    operator()(const dag::directed_edge<VertT>& e) const {
      return combine_hash(std::hash<VertT>{}(e.v1), e.v2);
    }
  };

  template<typename VertT>
  struct hash<dag::undirected_edge<VertT>> {
    size_t operator()(const dag::undirected_edge<VertT>& e) const {
      return unordered_hash(e.v1, e.v2);
    }
  };
}

namespace dag {
  template <typename VertT, typename TimeT>
  undirected_temporal_edge<VertT, TimeT>::undirected_temporal_edge(
      VertT v1, VertT v2, TimeT time) : v1(v1), v2(v2), time(time) {}

  template <typename VertT, typename TimeT>
  directed_temporal_edge<VertT, TimeT>::directed_temporal_edge(
      VertT v1, VertT v2, TimeT time) : v1(v1), v2(v2), time(time) {}

  template <typename VertT>
  std::ostream& operator<<(std::ostream& os,
      const directed_edge<VertT>& e) {
    return os << e.tail_vert() << " " << e.head_vert();
  }

  template <typename VertT>
  std::istream& operator>>(std::istream& is,
      directed_edge<VertT>& e) {
    return is >> e.v1 >> e.v2;
  }

  template <typename VertT>
  directed_edge<VertT>::directed_edge(VertT v1, VertT v2)
    : v1(v1), v2(v2) {}

  template <typename VertT>
  std::ostream& operator<<(std::ostream &os,
      const undirected_edge<VertT>& e) {
    return os << e.v1 << " " << e.v2;
  }

  template <typename VertT>
  std::istream& operator>>(std::istream& is,
      undirected_edge<VertT>& e) {
    return is >> e.v1 >> e.v2;
  }

  template <typename VertT>
  undirected_edge<VertT>::undirected_edge(VertT v1, VertT v2)
    : v1(v1), v2(v2) {}

  template <typename VertT, typename TimeT>
  std::ostream& operator<<(std::ostream& os,
      const undirected_temporal_edge<VertT, TimeT>& e) {
    return os <<  e.v1 << " " << e.v2 << " " << e.time;
  }

  template <typename VertT, typename TimeT>
  std::istream& operator>>(std::istream& is,
      undirected_temporal_edge<VertT, TimeT>& e) {
    return is >> e.v1 >> e.v2 >> e.time;
  }

  template <typename VertT, typename TimeT>
  std::ostream& operator<<(std::ostream& os,
      const directed_temporal_edge<VertT, TimeT>& e) {
    return os << e.v1 << " " << e.v2 << " " << e.time;
  }

  template <typename VertT, typename TimeT>
  std::istream& operator>>(std::istream& is,
      directed_temporal_edge<VertT, TimeT>& e) {
    return is >> e.v1 >> e.v2 >> e.time;
  }
}
