cmake_minimum_required(VERSION 3.12)

project(ppc2cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

# Dependency sadness ###########################################################

# CPM fetches and builds external dependencies
include(cmake/CPM.cmake)

# RVL binary parser
CPMAddPackage("gh:em-eight/ninutils#26a549969493bb7136cce2f38c14ab2b743c6882")
# ELF binary parser
CPMAddPackage("gh:serge1/ELFIO#8ae6cec")
# disassebler
CPMAddPackage("gh:em-eight/ppcdisasm-cpp#4e2c2e1")
# {fmt}
CPMAddPackage("gh:fmtlib/fmt#10.0.0")

# Import Protobuf
#
# There is a lot of pain behind this.
# The Protobuf repo itself and CMake provide their own ways
# to locate the library in CMake.  We are using FindProtobuf.cmake
# which is part of CMake since version 3.9.
#
# We are further using a static build, as dynamic linkage is very
# brittle.  On most systems, libprotobuf.a was compiled with
# libstdc++.  Therefore, ppc2cpp cannot support libc++ (part of LLVM).
set(Protobuf_USE_STATIC_LIBS ON)
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
  # protobuf_generate_cpp drops .pb.{h.c} files here.
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(ppc2cpp_core PUBLIC dolrel ppcdisasm elfio fmt ${Protobuf_LIBRARIES})

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
