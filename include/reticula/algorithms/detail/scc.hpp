#pragma once

#include <algorithm>
#include <cstddef>
#include <stack>
#include <unordered_map>

#include <reticula/components.hpp>
#include <reticula/concepts.hpp>

namespace reticula::detail {
/**
  Tarjan's algorithm for finding strongly connected components.
  @param net The network to analyze.
  @param singletons If true, singletons are included in the result.
  @return A vector of components, each representing a strongly connected
          component in the network. Components are returned in reverse
          topological order.
*/
template <network_like NetT>
auto scc_tarjan(const NetT& net, bool singletons) -> std::vector<component> {
  std::vector<component> result;
  component seen(net.vertices().size());

  std::unordered_map<VertexType, std::size_t> pre_order;
  pre_order.reserve(net.vertices().size());

  std::unordered_map<VertexType, std::size_t> root_order;
  root_order.reserve(net.vertices().size());

  std::stack<VertexType, std::vector<VertexType>> scc_stack;

  std::size_t current_pre = 0;
  for (auto source : net.vertices()) {
    if (seen.contains(source) || pre_order.contains(source))
      continue;

    std::stack<VertexType, std::vector<VertexType>> dfs;
    dfs.push(source);

    while (!dfs.empty()) {
      auto v = dfs.top();

      if (!pre_order.contains(v))
        pre_order[v] = current_pre++;

      auto succ = net.successors(v);
      auto it = std::ranges::find_if(
        succ, [&pre_order](VertexType s) { return !pre_order.contains(s); });
      if (it != succ.end()) {
        dfs.push(*it);
        continue;
      }

      auto& rv = root_order[v];
      rv = pre_order.at(v);

      for (auto s : succ) {
        if (!seen.contains(s)) {
          if (pre_order.at(s) > pre_order.at(v))
            rv = std::min(rv, root_order.at(s));
          else
            rv = std::min(rv, pre_order.at(s));
        }
      }

      dfs.pop();

      if (rv == pre_order.at(v)) {
        component comp;
        comp.insert(v);
        seen.insert(v);

        while (!scc_stack.empty() &&
               pre_order.at(scc_stack.top()) >= pre_order.at(v)) {
          auto s = scc_stack.top();
          scc_stack.pop();
          seen.insert(s);
          comp.insert(s);
        }

        if (singletons || comp.size() > 1)
          result.emplace_back(std::move(comp));
      } else {
        scc_stack.push(v);
      }
    }
  }

  return result;
}
} // namespace reticula::detail
