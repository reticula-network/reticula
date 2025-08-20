#pragma once

#include <reticula/networks.hpp>

auto create_random_directed_network(std::size_t n_vertices, std::size_t n_edges)
  -> reticula::directed_network;

auto create_strong_random_directed_network(
  std::size_t n_vertices, std::size_t n_edges) -> reticula::directed_network;

auto create_random_undirected_network(
  std::size_t n_vertices, std::size_t n_edges) -> reticula::undirected_network;

auto create_connected_random_undirected_network(
  std::size_t n_vertices, std::size_t n_edges) -> reticula::undirected_network;

auto create_random_dag_network(std::size_t n_vertices, std::size_t n_edges)
  -> reticula::directed_network;

auto create_bipartite_network(
  std::size_t n_vertices_a, std::size_t n_vertices_b, std::size_t n_edges)
  -> reticula::undirected_network;
