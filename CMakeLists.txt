cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
  reticula
  LANGUAGES CXX
  VERSION 0.2.0)

option(RETICULA_BUILD_TESTS "Build unit tests" "${PROJECT_IS_TOP_LEVEL}")
option(RETICULA_BUILD_BENCHMARKS "Build benchmarks" "${PROJECT_IS_TOP_LEVEL}")
option(RETICULA_ENABLE_WARNINGS "Enable warnings" on)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Dependencies

include(FetchContent)

FetchContent_Declare(
  hyperloglog
  GIT_REPOSITORY https://github.com/arashbm/hyperloglog.git
  GIT_SHALLOW TRUE
  GIT_TAG v1.0.1)
FetchContent_Declare(
  indexed_set
  GIT_REPOSITORY https://github.com/arashbm/indexed_set.git
  GIT_SHALLOW TRUE
  GIT_TAG v1.0.0)
FetchContent_Declare(
  disjoint_set
  GIT_REPOSITORY https://github.com/arashbm/disjoint_set.git
  GIT_SHALLOW TRUE
  GIT_TAG v1.0.0)
FetchContent_Declare(
  unordered_dense
  GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
  GIT_SHALLOW TRUE
  SYSTEM
  GIT_TAG v4.5.0)

FetchContent_MakeAvailable(disjoint_set hyperloglog indexed_set unordered_dense)

add_library(
  ${PROJECT_NAME}
  include/reticula/reticula.hpp
  src/stats.cpp
  src/networks.cpp
  # static edges
  src/edges/directed_edge.cpp
  src/edges/undirected_edge.cpp
  # static hyperedges
  src/edges/undirected_hyperedge.cpp
  src/edges/directed_hyperedge.cpp
  # temporal edges
  src/edges/undirected_temporal_edge.cpp
  src/edges/directed_temporal_edge.cpp
  src/edges/directed_delayed_temporal_edge.cpp
  # temporal hyperedges
  src/edges/undirected_temporal_hyperedge.cpp
  src/edges/directed_temporal_hyperedge.cpp
  src/edges/directed_delayed_temporal_hyperedge.cpp
  # algorithms
  src/algorithms/vertex_degree.cpp
  src/algorithms/edge_degree.cpp
  src/algorithms/graphicallity.cpp
  src/algorithms/properties.cpp
  src/algorithms/assortativity.cpp
  src/algorithms/static_reachability.cpp
  src/algorithms/temporal_reachability.cpp
  src/algorithms/dag.cpp
  src/algorithms/bipartite.cpp
  src/algorithms/distance.cpp
  # operations
  src/operations/algebraic.cpp
  src/operations/addition.cpp
  src/operations/removal.cpp
  src/operations/occupation.cpp
  src/operations/projections.cpp
  src/operations/subgraph.cpp
  src/operations/relabel.cpp
  # generation
  src/generation/deterministic.cpp
  src/generation/gnp.cpp
  src/generation/gnm.cpp
  src/generation/barabasi_albert.cpp
  src/generation/watts_strogatz.cpp
  src/generation/regular.cpp
  src/generation/expected_degree_sequence.cpp
  src/generation/degree_sequence.cpp
  src/generation/uniform_hypergraph.cpp
  src/generation/fully_mixed.cpp
  src/generation/link_activation.cpp
  src/generation/node_activation.cpp
  # others
  src/temporal_adjacency.cpp
  src/temporal_clusters.cpp
  src/event_graphs.cpp
  src/interval_sets.cpp
  src/components.cpp
  src/stats.cpp)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(${PROJECT_NAME} PUBLIC disjoint_set hyperloglog
                                             indexed_set unordered_dense)

include(CompilerWarnings)
enable_project_warnings(${PROJECT_NAME})

# Installation

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
  TARGETS ${PROJECT_NAME} unordered_dense
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT ${PROJECT_NAME}-targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

set(ConfigFile ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake)
configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}-config.cmake.in ${ConfigFile}
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES ${ConfigFile}
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Testing

if(RETICULA_BUILD_TESTS)
  enable_testing()

  add_subdirectory(tests)
endif()

# Benchmarking

if(RETICULA_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()

# Packaging helper

include(CPack)
