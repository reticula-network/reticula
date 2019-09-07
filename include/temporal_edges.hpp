#ifndef DAG_TEMPORAL_EDGES_H
#define DAG_TEMPORAL_EDGES_H

namespace dag {
  template <typename VertT, typename TimeT>
  class directed_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;
    VertT v1, v2;
    TimeT time;

    directed_temporal_edge() = default;

    directed_temporal_edge(const VertT v1, const VertT v2, const TimeT time)
      : v1(v1), v2(v2), time(time) {}

    [[nodiscard]]
    inline bool
    operator==(const directed_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(v1, v2, time);
      auto o = std::tie(other.v1, other.v2, other.time);
      return t == o;
    }

    [[nodiscard]]
    inline bool
    operator<(const directed_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(time, v1, v2);
      auto o = std::tie(other.time, other.v1, other.v2);
      return t < o;
    }

    [[nodiscard]]
    inline bool
    operator>(const directed_temporal_edge<VertT, TimeT>& other) const {
      return !(*this < other || *this == other);
    }

    [[nodiscard]]
    inline bool
    operator!=(const directed_temporal_edge<VertT, TimeT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline VertT head_vert() const { return v2; }

    [[nodiscard]]
    inline VertT tail_vert() const { return v1; }

    [[nodiscard]]
    inline TimeT effect_time() const { return time; }

    [[nodiscard]]
    inline bool is_out_incident(const VertT vert) const { return (v1 == vert); }

    [[nodiscard]]
    inline bool is_in_incident(const VertT vert) const  { return (v2 == vert); }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v2}; }

    [[nodiscard]]
    inline bool
    is_adjacent_to(const directed_temporal_edge<VertT, TimeT>& other) const {
      if ((other.time > time) && (head_vert() == other.tail_vert()))
        return true;
      else
        return false;
    }
  };

  template <typename VertT, typename TimeT>
  class directed_delayed_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;
    VertT v1, v2;
    TimeT time;
    TimeT delay;

    directed_delayed_temporal_edge() = default;

    directed_delayed_temporal_edge(
        const VertT v1, const VertT v2,
        const TimeT time, const TimeT delay)
      : v1(v1), v2(v2), time(time), delay(delay) {}

    [[nodiscard]]
    inline bool
    operator==(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(v1, v2, time, delay);
      auto o = std::tie(other.v1, other.v2, other.time, other.delay);
      return t == o;
    }

    [[nodiscard]]
    inline bool
    operator<(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
        auto t = std::tie(time, delay, v1, v2);
        auto o = std::tie(other.time, other.delay, other.v1, other.v2);
      return t < o;
    }

    [[nodiscard]]
    inline bool
    operator>(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
      return !(*this < other || *this == other);
    }

    [[nodiscard]]
    inline bool
    operator!=(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline VertT head_vert() const { return v2; }

    [[nodiscard]]
    inline VertT tail_vert() const { return v1; }

    [[nodiscard]]
    inline TimeT effect_time() const { return time+delay; }

    [[nodiscard]]
    inline bool is_out_incident(const VertT vert) const { return (v1 == vert); }

    [[nodiscard]]
    inline bool is_in_incident(const VertT vert) const  { return (v2 == vert); }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v2}; }

    [[nodiscard]]
    inline bool
    is_adjacent_to(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
      if ((other.time > effect_time()) && (head_vert() == other.tail_vert()))
        return true;
      else
        return false;
    }
  };

  template <typename VertT, typename TimeT>
  class undirected_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;
    VertT v1, v2;
    TimeT time;

    undirected_temporal_edge() = default;
    undirected_temporal_edge(const VertT v1, const VertT v2, const TimeT time)
      : v1(v1), v2(v2), time(time) {}

    [[nodiscard]]
    inline bool
    operator==(const undirected_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(v1, v2, time);
      auto o1 = std::tie(other.v1, other.v2, other.time);
      auto o2 = std::tie(other.v2, other.v1, other.time);
      return (t == o1 || t == o2);
    }

    [[nodiscard]]
    inline bool
    operator<(const undirected_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(time,
          std::min(v1, v2),
          std::max(v1, v2));
      auto o = std::tie(other.time,
          std::min(other.v1, other.v2),
          std::max(other.v1, other.v2));
      return (t < o);
    }

    [[nodiscard]]
    inline bool
    operator>(const undirected_temporal_edge<VertT, TimeT>& other) const {
      return !(*this < other || *this == other);
    }

    [[nodiscard]]
    inline bool
      operator!=(const undirected_temporal_edge<VertT, TimeT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline TimeT effect_time() const { return time; }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1, v2}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v1, v2}; }

    [[nodiscard]]
    inline bool
    is_adjacent_to(const undirected_temporal_edge<VertT, TimeT>& other) const {
      if ((other.time > time)
          && (v1 == other.v1 || v1 == other.v2 ||
                v2 == other.v1 || v2 == other.v2))
        return true;
      else
        return false;
    }
  };
}

#include "../src/temporal_edges.tpp"

#endif /* DAG_TEMPORAL_EDGES_H */
