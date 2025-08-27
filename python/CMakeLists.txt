include(CompilerWarnings)

find_package(
  Python 3.10 REQUIRED
  COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

if(NOT DEFINED SKBUILD)
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE nanobind_ROOT)
endif()

find_package(nanobind CONFIG REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

configure_file(include/version.hpp.in include/version.hpp @ONLY)

nanobind_add_module(
  _reticula_ext
  NB_DOMAIN
  reticula
  NB_SUPPRESS_WARNINGS
  NB_STATIC
  STABLE_ABI
  LTO
  src/random_state.cpp
  # edges
  src/edges/undirected_edge.cpp
  src/edges/directed_edge.cpp
  src/edges/undirected_temporal_edge.cpp
  src/edges/directed_temporal_edge.cpp
  src/edges/directed_delayed_temporal_edge.cpp
  src/edges/undirected_hyperedge.cpp
  src/edges/directed_hyperedge.cpp
  src/edges/undirected_temporal_hyperedge.cpp
  src/edges/directed_temporal_hyperedge.cpp
  src/edges/directed_delayed_temporal_hyperedge.cpp
  # networks
  src/networks.cpp
  # vectors
  src/vector.cpp
  # span
  src/span.cpp
  # main
  src/main.cpp)
target_link_libraries(_reticula_ext PRIVATE reticula)
target_include_directories(
  _reticula_ext
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
          $<INSTALL_INTERFACE:include>)
set_target_properties(_reticula_ext PROPERTIES POSITION_INDEPENDENT_CODE ON)
enable_project_warnings(_reticula_ext)
install(TARGETS _reticula_ext LIBRARY DESTINATION reticula)

nanobind_add_module(
  _reticula_generation
  NB_DOMAIN
  reticula
  NB_SUPPRESS_WARNINGS
  NB_STATIC
  STABLE_ABI
  LTO
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
  # main
  src/generation.cpp)
target_link_libraries(_reticula_generation PRIVATE reticula)
set_target_properties(_reticula_generation PROPERTIES POSITION_INDEPENDENT_CODE
                                                      ON)
enable_project_warnings(_reticula_generation)
install(TARGETS _reticula_generation LIBRARY DESTINATION reticula)

nanobind_add_module(
  _reticula_operations
  NB_DOMAIN
  reticula
  NB_SUPPRESS_WARNINGS
  NB_STATIC
  STABLE_ABI
  LTO
  # operations
  src/operations/algebraic.cpp
  src/operations/addition.cpp
  src/operations/removal.cpp
  src/operations/occupation.cpp
  src/operations/projections.cpp
  src/operations/subgraph.cpp
  src/operations/relabel.cpp
  # main
  src/operations.cpp)
target_link_libraries(_reticula_operations PRIVATE reticula)
set_target_properties(_reticula_operations PROPERTIES POSITION_INDEPENDENT_CODE
                                                      ON)

enable_project_warnings(_reticula_operations)
install(TARGETS _reticula_operations LIBRARY DESTINATION reticula)

nanobind_add_module(
  _reticula_algorithms
  NB_DOMAIN
  reticula
  NB_SUPPRESS_WARNINGS
  NB_STATIC
  STABLE_ABI
  LTO
  # operations
  src/algorithms/vertex_degree.cpp
  src/algorithms/edge_degree.cpp
  # main
  src/algorithms.cpp)
target_link_libraries(_reticula_algorithms PRIVATE reticula)
set_target_properties(_reticula_algorithms PROPERTIES POSITION_INDEPENDENT_CODE
                                                      ON)

enable_project_warnings(_reticula_operations)
install(TARGETS _reticula_algorithms LIBRARY DESTINATION reticula)
