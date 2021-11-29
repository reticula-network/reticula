cmake_minimum_required(VERSION 3.14)

project(dag LANGUAGES CXX VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(GNUInstallDirs)

include(FetchContent)

FetchContent_Declare(
  hyperloglog
  GIT_REPOSITORY https://github.com/arashbm/hyperloglog.git
  GIT_TAG ca74f23)
FetchContent_Declare(
  disjoint_set
  GIT_REPOSITORY https://github.com/arashbm/disjoint_set.git
  GIT_TAG 153c69d)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.7)

FetchContent_MakeAvailable(disjoint_set hyperloglog Catch2)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE
  $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>/include/
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${PROJECT_NAME} INTERFACE disjoint_set hyperloglog)


add_executable(
  ${PROJECT_NAME}_tests
  EXCLUDE_FROM_ALL
  src/test/dag/tests.cpp
  src/test/dag/adjacency_prob.cpp
  src/test/dag/algorithms.cpp
  src/test/dag/compile.cpp
  src/test/dag/edge_tests.cpp
  src/test/dag/estimators.cpp
  src/test/dag/implicit_event_graph.cpp
  src/test/dag/networks.cpp
  src/test/dag/temporal_components.cpp)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_tests PRIVATE Catch2::Catch2)
target_compile_options(${PROJECT_NAME}_tests PRIVATE
  -Werror -Wall -Wextra -Wconversion -Wsign-conversion)


install(
  TARGETS ${PROJECT_NAME}
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