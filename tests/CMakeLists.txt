include(CompilerWarnings)
include(Sanitizers)

include(FetchContent)

option(RETICULA_SANITIZE_TESTS "Build unit with sanitizer" on)

set(CATCH_INSTALL_DOCS
    OFF
    CACHE BOOL "" FORCE)
set(CATCH_INSTALL_EXTRAS
    OFF
    CACHE BOOL "" FORCE)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_SHALLOW TRUE
  GIT_TAG v3.9.0)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(
  ${PROJECT_NAME}_tests
  compile.cpp
  stats.cpp
  components.cpp
  # networks
  networks/directed.cpp
  networks/undirected.cpp
  networks/directed_hyper.cpp
  networks/undirected_hyper.cpp
  # temporal networks
  networks/directed_temporal.cpp
  networks/undirected_temporal.cpp
  networks/directed_delayed_temporal.cpp
  networks/directed_temporal_hyper.cpp
  networks/undirected_temporal_hyper.cpp
  networks/directed_delayed_temporal_hyper.cpp
  # static edges
  edges/undirected_edge.cpp
  edges/directed_edge.cpp
  # static hyperedges
  edges/undirected_hyperedge.cpp
  edges/directed_hyperedge.cpp
  # temporal edges
  edges/undirected_temporal_edge.cpp
  edges/directed_temporal_edge.cpp
  edges/directed_delayed_temporal_edge.cpp
  # temporal hyperedges
  edges/undirected_temporal_hyperedge.cpp
  edges/directed_temporal_hyperedge.cpp
  edges/directed_delayed_temporal_hyperedge.cpp
  # algorithms
  algorithms/vertex_degree.cpp
  algorithms/edge_degree.cpp
  algorithms/properties.cpp
  algorithms/graphicallity.cpp
  algorithms/assortativity.cpp
  algorithms/static_reachability.cpp
  algorithms/temporal_reachability.cpp
  algorithms/dag.cpp
  algorithms/bipartite.cpp
  algorithms/distance.cpp
  # operations
  operations/algebraic.cpp
  operations/projections.cpp
  operations/addition.cpp
  operations/removal.cpp
  operations/occupation.cpp
  operations/subgraph.cpp
  operations/relabel.cpp
  # generation
  generation/deterministic.cpp
  generation/gnp.cpp
  generation/gnm.cpp
  generation/barabasi_albert.cpp
  generation/watts_strogatz.cpp
  generation/regular.cpp
  generation/expected_degree_sequence.cpp
  generation/degree_sequence.cpp
  generation/uniform_hypergraph.cpp
  generation/fully_mixed.cpp
  generation/link_activation.cpp
  # views
  views/chain.cpp
  views/cartesian_product.cpp
  views/enumerate.cpp
  # misc
  temporal_clusters.cpp
  temporal_adjacency.cpp
  event_graphs.cpp
  interval_sets.cpp
  processes.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE Catch2::Catch2WithMain
                                                    ${PROJECT_NAME})

enable_project_warnings(${PROJECT_NAME}_tests)

if(RETICULA_SANITIZE_TESTS)
  enable_sanitizers(${PROJECT_NAME}_tests)
endif()

include(CTest)
include(Catch)

catch_discover_tests(${PROJECT_NAME}_tests)
