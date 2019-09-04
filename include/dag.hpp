#ifndef DAG_H
#define DAG_H

#include <unordered_set>
#include <unordered_map>
#include <random>
#include <iostream>
#include <stdexcept>

namespace dag {

  template <typename VertT>
  class directed_edge {
  public:
    using VertexType = VertT;
    VertT v1, v2;

    directed_edge() = default;
    directed_edge(const VertT v1, const VertT v2)
      : v1(v1), v2(v2) {}

    [[nodiscard]]
    inline bool operator==(const directed_edge<VertT>& other) const {
      auto t = std::tie(v1, v2);
      auto o = std::tie(other.v1, other.v2);
      return (t == o);
    }

    [[nodiscard]]
    inline bool operator!=(const directed_edge<VertT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline VertT head_vert() const { return v2; }

    [[nodiscard]]
    inline VertT tail_vert() const { return v1; }

    [[nodiscard]]
    inline bool is_out_incident(const VertT vert) const { return (v1 == vert); }

    [[nodiscard]]
    inline bool is_in_incident(const VertT vert) const  { return (v2 == vert); }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (is_out_incident(vert) || is_in_incident(vert));
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v2}; }
  };


  template <typename VertT>
  class undirected_edge {
  public:
    using VertexType = VertT;
    VertT v1, v2;

    undirected_edge() = default;
    undirected_edge(const VertT v1, const VertT v2)
      : v1(v1), v2(v2) {}

    [[nodiscard]]
    inline bool operator==(const undirected_edge<VertT>& other) const {
      auto t = std::tie(v1, v2);
      auto o1 = std::tie(other.v1, other.v2);
      auto o2 = std::tie(other.v2, other.v1);
      return (t == o1 || t == o2);
    }

    [[nodiscard]]
    inline bool operator!=(const undirected_edge<VertT>& other) const {
      return !(*this == other);
    }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1, v2}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v1, v2}; }
  };

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



  template <typename EdgeT>
  class network {
    std::unordered_set<EdgeT> edge_list;
  public:
    using VertexType = typename EdgeT::VertexType;
    using EdgeType = EdgeT;
    network() = default;
    std::unordered_set<VertexType> vertices() const;

    inline void add_edge(const EdgeT& new_edge) {
      edge_list.insert(new_edge);
    };

    void add_edges(const std::vector<EdgeT>& new_edges);

    inline const std::unordered_set<EdgeT>& edges() const { return edge_list; }
    inline void reserve(size_t count) { edge_list.reserve(count); }

    std::unordered_map<VertexType, std::unordered_set<EdgeT>>
      in_edges() const;
    std::unordered_map<VertexType, std::unordered_set<EdgeT>>
      out_edges() const;
    std::unordered_map<VertexType, std::unordered_set<EdgeT>>
      incident_edges() const;

    std::unordered_map<VertexType, size_t> in_degree() const;
    std::unordered_map<VertexType, size_t> out_degree() const;
    std::unordered_map<VertexType, size_t> degree() const;

    std::unordered_map<VertexType, std::unordered_set<VertexType>>
      predecessors() const;
    std::unordered_map<VertexType, std::unordered_set<VertexType>>
      successors() const;
    std::unordered_map<VertexType, std::unordered_set<VertexType>>
      neighbours() const;

    std::unordered_set<EdgeT> in_edges(const VertexType vert) const;
    std::unordered_set<EdgeT> out_edges(const VertexType vert) const;
    std::unordered_set<EdgeT> incident_edges(const VertexType vert) const;
  };

  template <typename VertT>
  using directed_network = network<directed_edge<VertT>>;

  template <typename VertT>
  using undirected_network = network<undirected_edge<VertT>>;

  template <typename VertT, typename TimeT>
  using directed_temporal_network = network<directed_temporal_edge<VertT, TimeT>>;

  template <typename VertT, typename TimeT>
  using directed_delayed_temporal_network = network<directed_delayed_temporal_edge<VertT, TimeT>>;

  template <typename VertT, typename TimeT>
  using undirected_temporal_network = network<undirected_temporal_edge<VertT, TimeT>>;

  template <typename VertT, typename TimeT>
  directed_network<undirected_temporal_edge<VertT, TimeT>>
  event_graph(const undirected_temporal_network<VertT, TimeT>& temp,
      TimeT max_delta_t);

  template <typename VertT, typename TimeT>
  directed_network<directed_temporal_edge<VertT, TimeT>>
  event_graph(const directed_temporal_network<VertT, TimeT>& temp,
      TimeT max_delta_t);

  template <typename VertT>
  std::vector<VertT> topological_order(const directed_network<VertT>& dir);

  class not_acyclic_error : public std::domain_error {
  public:
    explicit not_acyclic_error(const std::string& what_arg)
      : std::domain_error(what_arg) {};
    explicit not_acyclic_error(const char* what_arg)
      : std::domain_error(what_arg) {};
  };

  class vertex_type_too_small_error : public std::invalid_argument {
  public:
    explicit vertex_type_too_small_error(const std::string& what_arg)
      : std::invalid_argument(what_arg) {};
    explicit vertex_type_too_small_error(const char* what_arg)
      : std::invalid_argument(what_arg) {};
  };
};

#include "../src/utils.tpp"
#include "../src/edges.tpp"
#include "../src/network.tpp"
#include "../src/random_network.tpp"
#include "../src/algorithms.tpp"
#include "../src/implicit_event_graph.tpp"

#endif /* DAG_H */
