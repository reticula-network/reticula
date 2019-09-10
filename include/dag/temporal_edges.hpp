#ifndef DAG_TEMPORAL_EDGES_H
#define DAG_TEMPORAL_EDGES_H

namespace dag {
  template <typename VertT, typename TimeT>
  class directed_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;

    directed_temporal_edge() = default;

    directed_temporal_edge(const VertT v1, const VertT v2, const TimeT time)
      : v1(v1), v2(v2), time(time) {}

    [[nodiscard]]
    inline TimeT effect_time() const { return time; }

    [[nodiscard]]
    inline TimeT cause_time() const { return time; }

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
    friend bool adjacent(
        const directed_temporal_edge<VertT, TimeT>& a,
        const directed_temporal_edge<VertT, TimeT>& b) {
      return ((b.time > a.time) && (a.v2 == b.v1));
    }

    [[nodiscard]]
    friend bool operator!=(
        const directed_temporal_edge<VertT, TimeT>& a,
        const directed_temporal_edge<VertT, TimeT>& b){ return !(a == b); }

    [[nodiscard]]
    friend bool operator==(
        const directed_temporal_edge<VertT, TimeT>& a,
        const directed_temporal_edge<VertT, TimeT>& b){
      return (a.cause_comp_tuple() == b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool operator<(
        const directed_temporal_edge<VertT, TimeT>& a,
        const directed_temporal_edge<VertT, TimeT>& b){
      return (a.cause_comp_tuple() < b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool effect_lt(
        const directed_temporal_edge<VertT, TimeT>& a,
        const directed_temporal_edge<VertT, TimeT>& b){
      return (a.effect_comp_tuple() < b.effect_comp_tuple());
    }

    friend std::ostream& operator<<(std::ostream& os,
        const directed_temporal_edge<VertT, TimeT>& e) {
      return os << e.v1 << " " << e.v2 << " " << e.time;
    }

    friend std::istream& operator>>(std::istream& is,
        directed_temporal_edge<VertT, TimeT>& e) {
      return is >> e.v1 >> e.v2 >> e.time;
    }

  private:
    VertT v1, v2;
    TimeT time;

    inline std::tuple<TimeT, VertT, VertT> cause_comp_tuple() const {
      return std::make_tuple(time, v1, v2);
    }

    inline std::tuple<TimeT, VertT, VertT> effect_comp_tuple() const {
      return std::make_tuple(time, v2, v1);
    }

    friend struct std::hash<directed_temporal_edge<VertT, TimeT>>;
  };

  template <typename VertT, typename TimeT>
  class directed_delayed_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;

    directed_delayed_temporal_edge() = default;

    directed_delayed_temporal_edge(
        const VertT v1, const VertT v2,
        const TimeT time, const TimeT delay)
      : v1(v1), v2(v2), time(time), delay(delay) {}

    [[nodiscard]]
    inline TimeT effect_time() const { return time+delay; }

    [[nodiscard]]
    inline TimeT cause_time() const { return time; }

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
    friend bool adjacent(
        const directed_delayed_temporal_edge<VertT, TimeT>& a,
        const directed_delayed_temporal_edge<VertT, TimeT>& b) {
      return ((b.time > a.effect_time()) && (a.v2 == b.v1));
    }

    [[nodiscard]]
    friend bool operator!=(
        const directed_delayed_temporal_edge<VertT, TimeT>& a,
        const directed_delayed_temporal_edge<VertT, TimeT>& b){ return !(a == b); }

    [[nodiscard]]
    friend bool operator==(
        const directed_delayed_temporal_edge<VertT, TimeT>& a,
        const directed_delayed_temporal_edge<VertT, TimeT>& b){
      return (a.cause_comp_tuple() == b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool operator<(
        const directed_delayed_temporal_edge<VertT, TimeT>& a,
        const directed_delayed_temporal_edge<VertT, TimeT>& b){
      return (a.cause_comp_tuple() < b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool effect_lt(
        const directed_delayed_temporal_edge<VertT, TimeT>& a,
        const directed_delayed_temporal_edge<VertT, TimeT>& b){
      return (a.effect_comp_tuple() < b.effect_comp_tuple());
    }

    friend std::ostream& operator<<(std::ostream& os,
        const directed_delayed_temporal_edge<VertT, TimeT>& e) {
      return os << e.v1 << " " << e.v2 << " " << e.time << " " << e.delay;
    }

    friend std::istream& operator>>(std::istream& is,
        directed_delayed_temporal_edge<VertT, TimeT>& e) {
      return is >> e.v1 >> e.v2 >> e.time >> e.delay;
    }

  private:
    VertT v1, v2;
    TimeT time, delay;

    inline std::tuple<TimeT, TimeT, VertT, VertT> cause_comp_tuple() const {
      return std::make_tuple(time, time+delay, v1, v2);
    }

    inline std::tuple<TimeT, TimeT, VertT, VertT> effect_comp_tuple() const {
      return std::make_tuple(time+delay, time, v2, v1);
    }

    friend struct std::hash<directed_delayed_temporal_edge<VertT, TimeT>>;
  };

  template <typename VertT, typename TimeT>
  class undirected_temporal_edge {
  public:
    using VertexType = VertT;
    using TimeType = TimeT;

    undirected_temporal_edge() = default;
    undirected_temporal_edge(const VertT v1, const VertT v2, const TimeT time)
      : v1(v1), v2(v2), time(time) {}


    [[nodiscard]]
    inline TimeT effect_time() const { return time; }

    [[nodiscard]]
    inline TimeT cause_time() const { return time; }

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1, v2}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v1, v2}; }

    [[nodiscard]]
    friend bool adjacent(
        const undirected_temporal_edge<VertT, TimeT>& a,
        const undirected_temporal_edge<VertT, TimeT>& b) {
      return ((b.time > a.time) &&
                (a.v1 == b.v1 ||
                 a.v1 == b.v2 ||
                 a.v2 == b.v1 ||
                 a.v2 == b.v2));
    }


    [[nodiscard]]
    friend bool operator!=(
        const undirected_temporal_edge<VertT, TimeT>& a,
        const undirected_temporal_edge<VertT, TimeT>& b){ return !(a == b); }

    [[nodiscard]]
    friend bool operator==(
        const undirected_temporal_edge<VertT, TimeT>& a,
        const undirected_temporal_edge<VertT, TimeT>& b){
      return (a.comp_tuple() == b.comp_tuple());
    }

    [[nodiscard]]
    friend bool operator<(
        const undirected_temporal_edge<VertT, TimeT>& a,
        const undirected_temporal_edge<VertT, TimeT>& b){
      return (a.comp_tuple() < b.comp_tuple());
    }

    [[nodiscard]]
    friend bool effect_lt(
        const undirected_temporal_edge<VertT, TimeT>& a,
        const undirected_temporal_edge<VertT, TimeT>& b){
      return (a < b);
    }

    friend std::ostream& operator<<(std::ostream& os,
        const undirected_temporal_edge<VertT, TimeT>& e) {
      return os <<  e.v1 << " " << e.v2 << " " << e.time;
    }

    friend std::istream& operator>>(std::istream& is,
        undirected_temporal_edge<VertT, TimeT>& e) {
      return is >> e.v1 >> e.v2 >> e.time;
    }

  private:
    VertT v1, v2;
    TimeT time;

    inline std::tuple<TimeT, VertT, VertT> comp_tuple() const {
      return std::make_tuple(time, std::min(v1, v2), std::max(v1, v2));
    }

    friend struct std::hash<undirected_temporal_edge<VertT, TimeT>>;
  };
}

#include "../../src/temporal_edges.tpp"

#endif /* DAG_TEMPORAL_EDGES_H */
