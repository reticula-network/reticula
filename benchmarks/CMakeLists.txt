include(CompilerWarnings)
include(FetchContent)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

set(BENCHMARK_ENABLE_TESTING
    OFF
    CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL
    OFF
    CACHE BOOL "" FORCE)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.4
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(benchmark)

# Benchmark executable
add_executable(
  reticula_benchmarks
  random_network.cpp
  algorithms/vertex_degree.cpp
  algorithms/edge_degree.cpp
  algorithms/static_reachability.cpp
  algorithms/properties.cpp
  algorithms/assortativity.cpp
  algorithms/bipartite.cpp
  algorithms/dag.cpp
  algorithms/graphicallity.cpp
  algorithms/distance.cpp
  networks/directed.cpp
  networks/undirected.cpp
  networks/temporal.cpp)

target_link_libraries(
  reticula_benchmarks PRIVATE reticula::reticula benchmark::benchmark
                              benchmark::benchmark_main)

target_include_directories(reticula_benchmarks
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

enable_project_warnings(reticula_benchmarks)

# Add custom target to run benchmarks
add_custom_target(
  run_benchmarks
  COMMAND reticula_benchmarks
  DEPENDS reticula_benchmarks
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running benchmarks")
