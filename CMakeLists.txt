cmake_minimum_required(VERSION 3.12)

project(ppc2cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

# CPM fetches and builds external dependencies
include(cmake/CPM.cmake)

# RVL binary parser
CPMAddPackage("gh:em-eight/ninutils#dc8939fcf90d265706da6422b204e8c03fb85e7f")
# ELF binary parser
CPMAddPackage("gh:serge1/ELFIO#8ae6cec")
# disassebler
CPMAddPackage(
  NAME ppcdisasm-cpp
  OPTIONS "BUILD_TESTING OFF"
  GITHUB_REPOSITORY "em-eight/ppcdisasm-cpp"
  GIT_TAG "d7424cb"
)
# yaml
CPMAddPackage(
  NAME yaml-cpp
  GITHUB_REPOSITORY jbeder/yaml-cpp
  VERSION 0.7.0
  GIT_TAG yaml-cpp-0.7.0
  OPTIONS
    "YAML_CPP_BUILD_TESTS Off"
    "YAML_CPP_BUILD_CONTRIB Off"
    "YAML_CPP_BUILD_TOOLS Off"
)
# {fmt}
CPMAddPackage("gh:fmtlib/fmt#10.0.0")

set(Protobuf_USE_STATIC_LIBS ON)
set(VCPKG_LIBRARY_LINKAGE static) # for vcpkg

find_package(protobuf CONFIG)
if(NOT protobuf_FOUND)        # Legacy protobuf support
  message(STATUS "Trying legacy method of finding protobuf")
  set(protobuf_MODULE_COMPATIBLE ON)
  include(FindProtobuf)
  find_package(Protobuf REQUIRED)
endif()

# Generate Protobuf files
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
file(GLOB PROTO_FILES "${PROTO_DIR}/*.proto")
if(protobuf_MODULE_COMPATIBLE)
  protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
endif()

# ppc2cpp core #################################################################

file(GLOB_RECURSE ppc2cpp_core_SRC "src/*.cpp")

if(protobuf_MODULE_COMPATIBLE)
  add_library(ppc2cpp_core ${ppc2cpp_core_SRC} ${PROTO_SRCS} ${PROTO_HDRS} )
  target_include_directories(ppc2cpp_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # <prefix>/include
    ${YAML_CPP_SOURCE_DIR}/include
    # protobuf_generate_cpp drops .pb.{h.c} files here.
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
  )
  target_link_libraries(ppc2cpp_core dolrel ppcdisasm elfio fmt ${PROTOBUF_LIBRARIES} yaml-cpp)
  get_target_property(OUT ppc2cpp_core LINK_LIBRARIES)
  message(STATUS ${OUT})
else()
  add_library(ppc2cpp_core ${ppc2cpp_core_SRC} ${PROTO_FILES})
  target_include_directories(ppc2cpp_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>  # <prefix>/include
    ${YAML_CPP_SOURCE_DIR}/include
    # protobuf_generate_cpp drops .pb.{h.c} files here.
    ${CMAKE_CURRENT_BINARY_DIR}
  )
  target_link_libraries(ppc2cpp_core dolrel ppcdisasm elfio fmt protobuf::libprotobuf yaml-cpp)
  protobuf_generate(
    TARGET ppc2cpp_core
    IMPORT_DIRS "${PROTO_DIR}"
  )
endif()

# ppc2cpp CLI  #################################################################

add_executable(ppc2cpp src/cli/ppc2cpp.cpp)
target_include_directories(ppc2cpp PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(ppc2cpp ppc2cpp_core)

# project_demo #################################################################

add_executable(project_demo test/program_loader_demo.cpp)
target_include_directories(project_demo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(project_demo ppc2cpp_core)

# Install ######################################################################

# libs
install(TARGETS ppc2cpp_core EXPORT export_ppc2cpp
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# executables
install(TARGETS ppc2cpp DESTINATION bin)

# headers
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

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
