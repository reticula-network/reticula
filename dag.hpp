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
    directed_edge(const VertT v1, const VertT v2);

    inline bool operator==(const directed_edge<VertT>& other) const {
      auto t = std::tie(v1, v2);
      auto o = std::tie(other.v1, other.v2);
      return (t == o);
    }

    inline bool operator!=(const directed_edge<VertT>& other) const {
      return !(this == other);
    }

    inline VertT head_vert() const {
      return v2;
    }

    inline VertT tail_vert() const {
      return v1;
    }


    inline bool is_out_incident(const VertT vert) const {
      return (v1 == vert);
    }
    inline bool is_in_incident(const VertT vert) const  {
      return (v2 == vert);
    }
    inline bool is_incident(const VertT vert) const {
      return (is_out_incident(vert) || is_in_incident(vert));
    }

    std::vector<VertT> mutator_verts() const {
      return {v1};
    }

    std::vector<VertT> mutated_verts() const {
      return {v2};
    }
  };


  template <typename VertT>
  class undirected_edge {
  public:
    using VertexType = VertT;
    VertT v1, v2;
    undirected_edge(const VertT v1, const VertT v2);

    inline bool operator==(const undirected_edge<VertT>& other) const {
      auto t = std::tie(v1, v2);
      auto o1 = std::tie(other.v1, other.v2);
      auto o2 = std::tie(other.v2, other.v1);
      return (t == o1 || t == o2);
    }

    inline bool operator!=(const undirected_edge<VertT>& other) const {
      return !(this == other);
    }


    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    std::vector<VertT> mutator_verts() const {
      return {v1, v2};
    }

    std::vector<VertT> mutated_verts() const {
      return {v1, v2};
    }
  };

  template <typename VertT, typename TimeT>
  class directed_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;
    VertT v1, v2;
    TimeT time;

    directed_temporal_edge() = default;

    directed_temporal_edge(const VertT v1, const VertT v2, const TimeT time);

    inline bool
      operator==(const directed_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(v1, v2, time);
      auto o = std::tie(other.v1, other.v2, other.time);
      return t == o;
    }

    inline bool
      operator<(const directed_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(time, v1, v2);
      auto o = std::tie(other.time, other.v1, other.v2);
      return t < o;
    }

    inline bool
      operator!=(const directed_temporal_edge<VertT, TimeT>& other) const {
      return !(this == other);
    }

    inline VertT head_vert() const {
      return v2;
    }
    inline VertT tail_vert() const {
      return v1;
    }

    inline TimeT effect_time() const {
      return time;
    }

    inline bool is_out_incident(const VertT vert) const {
      return (v1 == vert);
    }
    inline bool is_in_incident(const VertT vert) const  {
      return (v2 == vert);
    }
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    std::vector<VertT> mutator_verts() const {
      return {v1};
    }

    std::vector<VertT> mutated_verts() const {
      return {v2};
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
        const TimeT time, const TimeT delay);

    inline bool
      operator==(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(v1, v2, time, delay);
      auto o = std::tie(other.v1, other.v2, other.time, other.delay);
      return t == o;
    }

    inline bool
      operator<(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
        auto t = std::tie(time, delay, v1, v2);
        auto o = std::tie(other.time, other.delay, other.v1, other.v2);
      return t < o;
    }

    inline bool
      operator!=(const directed_delayed_temporal_edge<VertT, TimeT>& other) const {
      return !(this == other);
    }

    inline VertT head_vert() const {
      return v2;
    }
    inline VertT tail_vert() const {
      return v1;
    }

    inline TimeT effect_time() const {
      return time+delay;
    }

    inline bool is_out_incident(const VertT vert) const {
      return (v1 == vert);
    }
    inline bool is_in_incident(const VertT vert) const  {
      return (v2 == vert);
    }
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    std::vector<VertT> mutator_verts() const {
      return {v1};
    }

    std::vector<VertT> mutated_verts() const {
      return {v2};
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
    undirected_temporal_edge(const VertT v1, const VertT v2, const TimeT time);

    inline bool
      operator==(const undirected_temporal_edge<VertT, TimeT>& other) const {
      auto t = std::tie(v1, v2, time);
      auto o1 = std::tie(other.v1, other.v2, other.time);
      auto o2 = std::tie(other.v2, other.v1, other.time);
      return (t == o1 || t == o2);
    }

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

    inline bool
      operator!=(const undirected_temporal_edge<VertT, TimeT>& other) const {
      return !(this == other);
    }

    inline TimeT effect_time() const {
      return time;
    }

    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    std::vector<VertT> mutator_verts() const {
      return {v1, v2};
    }

    std::vector<VertT> mutated_verts() const {
      return {v1, v2};
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

#include "edges.tpp"
#include "network.tpp"
#include "random_network.tpp"
#include "algorithms.tpp"

#endif /* DAG_H */
