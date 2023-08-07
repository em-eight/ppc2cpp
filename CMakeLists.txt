cmake_minimum_required(VERSION 3.12)

project(ppc2cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

# Dependency sadness ###########################################################

# CPM fetches and builds external dependencies
include(cmake/CPM.cmake)

# RVL binary parser
CPMAddPackage("gh:em-eight/ninutils#9c52c522b92ef4400d6a98b58ab9b2ae95f14252")
# ELF binary parser
CPMAddPackage("gh:serge1/ELFIO#8ae6cec")
# disassebler
CPMAddPackage("gh:em-eight/ppcdisasm-cpp#4e2c2e1")
# yaml
CPMAddPackage("gh:jbeder/yaml-cpp#yaml-cpp-0.7.0")
# {fmt}
CPMAddPackage("gh:fmtlib/fmt#10.0.0")

# Import Protobuf
#
# There is a lot of pain behind this.
# The Protobuf repo itself and CMake provide their own ways
# to locate the library in CMake.  We are using FindProtobuf.cmake
# which is part of CMake since version 3.9.

# If the Protobuf_LIBRARY workaround is used (see README), the user
# could either specify a static or shared library.  Depending on either,
# we need to correctly set the link mode before calling find_package.
# Newer versions of Protobuf depend on Abseil.
if(DEFINED Protobuf_LIBRARY)
  if(Protobuf_LIBRARY MATCHES "\.a$")
    set(Protobuf_USE_STATIC_LIBS ON)
  else()
    set(Protobuf_USE_STATIC_LIBS OFF)
  endif()
endif()

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# Generate Protobuf files
# This is provided by FindProtobuf.cmake
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# ppc2cpp core #################################################################

file(GLOB_RECURSE ppc2cpp_core_SRC "src/*.cpp")
add_library(ppc2cpp_core ${ppc2cpp_core_SRC} ${PROTO_SRCS} ${PROTO_HDRS} )
target_include_directories(ppc2cpp_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
  ${YAML_CPP_INCLUDE_DIRS}
  # protobuf_generate_cpp drops .pb.{h.c} files here.
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(ppc2cpp_core PUBLIC dolrel ppcdisasm elfio fmt ${Protobuf_LIBRARIES} ${YAML_CPP_LIBRARIES})

# project_demo #################################################################

add_executable(project_demo test/program_loader_demo.cpp)
target_include_directories(project_demo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(project_demo ppc2cpp_core)

# Tests ########################################################################

if(BUILD_TESTING)
CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  GIT_TAG release-1.12.1
  VERSION 1.12.1
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
)

add_executable(analysis_test test/analysis_test.cpp)
target_include_directories(analysis_test PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(
  analysis_test
  GTest::gtest_main
  ppc2cpp_core
)
target_compile_definitions(analysis_test PRIVATE -DTEST_PATH="${CMAKE_SOURCE_DIR}/test")

enable_testing()
add_test(analysis_test analysis_test)
endif()
