#ifndef DAG_STATIC_EDGES_H
#define DAG_STATIC_EDGES_H

namespace dag {
  template <typename VertT>
  class directed_edge {
  public:
    using VertexType = VertT;

    directed_edge() = default;
    directed_edge(const VertT v1, const VertT v2)
      : v1(v1), v2(v2) {}


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

    [[nodiscard]]
    friend bool operator!=(
        const directed_edge<VertT>& a,
        const directed_edge<VertT>& b){ return !(a == b); }

    [[nodiscard]]
    friend bool operator==(
        const directed_edge<VertT>& a,
        const directed_edge<VertT>& b){
      return (a.cause_comp_tuple() == b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool operator<(
        const directed_edge<VertT>& a,
        const directed_edge<VertT>& b){
      return (a.cause_comp_tuple() < b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool effect_lt(
        const directed_edge<VertT>& a,
        const directed_edge<VertT>& b){
      return (a.effect_comp_tuple() < b.effect_comp_tuple());
    }

    [[nodiscard]]
    friend bool adjacent(
        const directed_edge<VertT>& a,
        const directed_edge<VertT>& b) {
      return (a.v2 == b.v1);
    }

    friend std::ostream& operator<<(std::ostream &os,
        directed_edge<VertT>& e) {
      return os << e.v1 << " " << e.v2;
    }
    friend std::istream& operator>>(std::istream &is,
        directed_edge<VertT>& e) {
      return is >> e.v1 >> e.v2;
    }

  private:
    VertT v1, v2;
    [[nodiscard]]
    inline std::tuple<VertT, VertT> cause_comp_tuple() const {
      return std::make_tuple(v1, v2);
    }

    [[nodiscard]]
    inline std::tuple<VertT, VertT> effect_comp_tuple() const {
      return std::make_tuple(v2, v1);
    }

    friend struct std::hash<directed_edge<VertT>>;
  };


  template <typename VertT>
  class undirected_edge {
  public:
    using VertexType = VertT;

    undirected_edge() = default;
    undirected_edge(const VertT v1, const VertT v2)
      : v1(v1), v2(v2) {}

    [[nodiscard]]
    inline bool is_incident(const VertT vert) const {
      return (v1 == vert || v2 == vert);
    }

    [[nodiscard]]
    std::vector<VertT> mutator_verts() const { return {v1, v2}; }

    [[nodiscard]]
    std::vector<VertT> mutated_verts() const { return {v1, v2}; }

    [[nodiscard]]
    friend bool operator!=(
        const undirected_edge<VertT>& a,
        const undirected_edge<VertT>& b){ return !(a == b); }

    [[nodiscard]]
    friend bool operator==(
        const undirected_edge<VertT>& a,
        const undirected_edge<VertT>& b){
      return (a.comp_tuple() == b.comp_tuple());
    }

    [[nodiscard]]
    friend bool operator<(
        const undirected_edge<VertT>& a,
        const undirected_edge<VertT>& b){
      return (a.comp_tuple() < b.comp_tuple());
    }

    [[nodiscard]]
    friend bool effect_lt(
        const undirected_edge<VertT>& a,
        const undirected_edge<VertT>& b){
      return (a < b);
    }

    [[nodiscard]]
    friend bool adjacent(
        const undirected_edge<VertT>& a,
        const undirected_edge<VertT>& b) {
      return (a.v1 == b.v1 ||
              a.v1 == b.v2 ||
              a.v2 == b.v1 ||
              a.v2 == b.v2);
    }

    friend std::ostream& operator<<(std::ostream &os,
        undirected_edge<VertT>& e) {
      return os << e.v1 << " " << e.v2;
    }

    friend std::istream& operator>>(std::istream &is,
        undirected_edge<VertT>& e) {
      return is >> e.v1 >> e.v2;
    }

  private:
    VertT v1, v2;
    [[nodiscard]]
    inline std::tuple<VertT, VertT> comp_tuple() const {
      return std::make_tuple(std::min(v1, v2), std::max(v1, v2));
    }

    friend struct std::hash<undirected_edge<VertT>>;
  };
}

#include "../../src/static_edges.tpp"

#endif /* DAG_STATIC_EDGES_H */
