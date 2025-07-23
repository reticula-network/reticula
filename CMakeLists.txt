cmake_minimum_required(VERSION 3.14)
project(reticula LANGUAGES CXX VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

get_directory_property(is_subproject PARENT_DIRECTORY)
if(NOT is_subproject)
  set(is_standalone YES)
else()
  set(is_standalone NO)
endif()

include(GNUInstallDirs)

include(FetchContent)

FetchContent_Declare(
  hyperloglog
  GIT_REPOSITORY https://github.com/arashbm/hyperloglog.git
  GIT_SHALLOW TRUE
  GIT_TAG v1.0.0)
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
  csv
  GIT_REPOSITORY https://github.com/arashbm/csv-parser.git
  GIT_TAG 8bda02dabbb6b3b64d24841efee912c0a17fdf00
  GIT_SHALLOW TRUE)
FetchContent_Declare(
  json
  URL_HASH MD5=e155202b2a589137f6804724bd182f12
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP true)

FetchContent_MakeAvailable(disjoint_set hyperloglog indexed_set csv json)

add_library(csv_system INTERFACE)
target_link_libraries(csv_system INTERFACE csv)

get_target_property(csv_includes csv INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(csv_system SYSTEM INTERFACE "${csv_includes}")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>/include/
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} INTERFACE
  disjoint_set hyperloglog indexed_set csv_system nlohmann_json::nlohmann_json)

if (is_standalone)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1)

  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  set(TEST_FILES
    src/test/reticula/stats.cpp
    src/test/reticula/utils.cpp
    src/test/reticula/intervals.cpp
    src/test/reticula/temporal_adjacency.cpp
    src/test/reticula/operations.cpp
    src/test/reticula/algorithms.cpp
    src/test/reticula/temporal_algorithms.cpp
    src/test/reticula/compile.cpp
    src/test/reticula/edge_tests.cpp
    src/test/reticula/generators.cpp
    src/test/reticula/implicit_event_graphs.cpp
    src/test/reticula/implicit_event_graph_components.cpp
    src/test/reticula/networks.cpp
    src/test/reticula/io.cpp
    src/test/reticula/random_networks.cpp
    src/test/reticula/distributions.cpp
    src/test/reticula/components.cpp
    src/test/reticula/temporal_clusters.cpp
    src/test/reticula/microcanonical_reference_models.cpp
    src/test/reticula/traits.cpp)
    

  add_executable(${PROJECT_NAME}_tests EXCLUDE_FROM_ALL ${TEST_FILES})
  target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE ${PROJECT_NAME} Catch2::Catch2WithMain)
  target_compile_options(reticula_tests PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Werror -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic
        -Wnull-dereference -Wold-style-cast -Wcast-align -Wunused
        -Woverloaded-virtual -Wimplicit-fallthrough
        -Wmisleading-indentation
        $<$<CONFIG:Debug>:-g -fsanitize=address,undefined>>
    $<$<CXX_COMPILER_ID:GNU>:
        -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast>
    $<$<CXX_COMPILER_ID:MSVC>:
        /bigobj /W4 /permissive- /w14242 /w14254 /w14263 /w14265 /w14287
        /we4289 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555
        /w14619 /w14640 /w14826 /w14905 /w14906 /w14928>)

  target_link_options(reticula_tests PRIVATE
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:
        -fsanitize=address,undefined>)

  include(CTest)
  include(Catch)
  catch_discover_tests(${PROJECT_NAME}_tests EXTRA_ARGS --skip-benchmarks)

  add_executable(${PROJECT_NAME}_benchmarks EXCLUDE_FROM_ALL ${TEST_FILES})
  target_link_libraries(${PROJECT_NAME}_benchmarks
    PRIVATE ${PROJECT_NAME} Catch2::Catch2WithMain)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME}_benchmarks PRIVATE /bigobj)
  endif()
endif()

install(
  TARGETS ${PROJECT_NAME} csv_system csv nlohmann_json
  EXPORT ${PROJECT_NAME}_Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${PROJECT_NAME}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

if(NOT INCLUDE_INSTALL_DIR)
  set(INCLUDE_INSTALL_DIR include/)
endif()

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
  PATH_VARS INCLUDE_INSTALL_DIR)

install(
  EXPORT ${PROJECT_NAME}_Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(
  FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
