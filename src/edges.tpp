#include <functional>

// Hashing rules for edges
namespace std {
  template<typename VertT, typename TimeT>
  struct hash<dag::undirected_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::undirected_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
          dag::utils::unordered_hash(e.v1, e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::directed_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
          combine_hash(std::hash<VertT>{}(e.v1), e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_delayed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::directed_delayed_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
              dag::utils::combine_hash(
                dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2),
                e.time),
              e.delay);
    }
  };

  template<typename VertT>
  struct hash<dag::directed_edge<VertT>> {
    size_t
    operator()(const dag::directed_edge<VertT>& e) const {
      return dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2);
    }
  };

  template<typename VertT>
  struct hash<dag::undirected_edge<VertT>> {
    size_t operator()(const dag::undirected_edge<VertT>& e) const {
      return dag::utils::unordered_hash(e.v1, e.v2);
    }
  };
}

namespace dag {
  // directed edge IO:
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


  // undirected edge IO:
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


  // undirected temporal edge IO:
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

  // directed temporal edge IO:
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

  // directed delayed temporal edge IO:
  template <typename VertT, typename TimeT>
  std::ostream& operator<<(std::ostream& os,
      const directed_delayed_temporal_edge<VertT, TimeT>& e) {
    return os << e.v1 << " " << e.v2 << " " << e.time << " " << e.delay;
  }

  template <typename VertT, typename TimeT>
  std::istream& operator>>(std::istream& is,
      directed_delayed_temporal_edge<VertT, TimeT>& e) {
    return is >> e.v1 >> e.v2 >> e.time >> e.delay;
  }
}
