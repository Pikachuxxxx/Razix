cmake_minimum_required(VERSION 3.20)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
    "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "")
endif()

message(STATUS "TOOLCHAIN: ${CMAKE_TOOLCHAIN_FILE}")

project(FrameGraph)

if(PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
  message(
    FATAL_ERROR
    "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .lib
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # .dll
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin") # .exe
endif()

option(FG_BUILD_TEST "Build unit tests" ON)

add_library(
  FrameGraph
  "include/fg/TypeTraits.hpp"
  "include/fg/FrameGraphResource.hpp"
  "include/fg/FrameGraph.hpp"
  "include/fg/FrameGraph.inl"
  "include/fg/GraphNode.hpp"
  "include/fg/PassNode.hpp"
  "include/fg/PassEntry.hpp"
  "include/fg/ResourceNode.hpp"
  "include/fg/ResourceEntry.hpp"
  "include/fg/ResourceEntry.inl"
  "include/fg/Blackboard.hpp"
  "include/fg/Blackboard.inl"
  "include/fg/Fwd.hpp"
  "src/FrameGraph.cpp"
  "src/GraphNode.cpp"
  "src/PassNode.cpp"
  "src/ResourceNode.cpp"
  "src/ResourceEntry.cpp")

target_include_directories(
  FrameGraph PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

add_library(fg::FrameGraph ALIAS FrameGraph)

if(FG_BUILD_TEST)
  enable_testing()
  add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/fg
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
