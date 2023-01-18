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
  GIT_TAG 22f39d0)
FetchContent_Declare(
  disjoint_set
  GIT_REPOSITORY https://github.com/arashbm/disjoint_set.git
  GIT_TAG 18aa052)
FetchContent_Declare(
  Range-v3
  GIT_REPOSITORY https://github.com/ericniebler/range-v3.git
  GIT_TAG 0.12.0)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.2.1)

FetchContent_MakeAvailable(disjoint_set hyperloglog)

add_library(csv INTERFACE)
target_include_directories(csv
  SYSTEM INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>/extern/csv/
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>/include/
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} INTERFACE
  disjoint_set hyperloglog csv)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(
  ${PROJECT_NAME}_tests
  EXCLUDE_FROM_ALL
  src/test/reticula/stats.cpp
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
  src/test/reticula/microcanonical_reference_models.cpp)
target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE ${PROJECT_NAME} Catch2::Catch2WithMain)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME}_tests PRIVATE
    /W4 /bigobj /permissive-)
else()
  target_compile_options(${PROJECT_NAME}_tests PRIVATE
    -Werror -Wall -Wextra -Wconversion -Wsign-conversion
    -fsanitize=address,undefined -g
    -Wno-deprecated-declarations) # std::tmpnam in tests/io.cpp
  target_link_options(${PROJECT_NAME}_tests PRIVATE
    -fsanitize=address,undefined)
endif()


if (is_standalone)
  include(CTest)
  include(Catch)
  catch_discover_tests(${PROJECT_NAME}_tests)
endif()

install(
  TARGETS ${PROJECT_NAME} csv
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
