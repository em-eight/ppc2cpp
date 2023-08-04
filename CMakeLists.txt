cmake_minimum_required(VERSION 3.12)

project(ppc2cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)
include(FindProtobuf REQUIRED)

# RVL binary parser
CPMAddPackage("gh:em-eight/ninutils#b877507806070d0393b3f8d89ea242885a0bbae2")
# ELF binary parser
CPMAddPackage("gh:serge1/ELFIO#8ae6cec")
# disassebler
CPMAddPackage("gh:em-eight/ppcdisasm-cpp#4e2c2e1")

# Proto stuff
find_package(Protobuf REQUIRED)

set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
file(GLOB PROTO_FILES "${PROTO_PATH}/*.proto")
foreach(PROTO_FILE ${PROTO_FILES})
  string(REGEX REPLACE "[.]proto$" ".pb.cc" PROTO_SOURCE ${PROTO_FILE})
  list(APPEND PROTO_SOURCES ${PROTO_SOURCE}) 
endforeach()

set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_DEST ${CMAKE_CURRENT_SOURCE_DIR}/proto)

add_custom_command(OUTPUT ${PROTO_SOURCES}
                   COMMAND protoc --proto_path=${PROTO_PATH} --cpp_out=${PROTO_DEST} ${PROTO_FILES}
                   DEPENDS ${PROTO_FILES}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMENT "PROTOCC")

file(GLOB_RECURSE ppc2cpp_core_SRC
     "src/*.cpp"
)
add_library(ppc2cpp_core ${ppc2cpp_core_SRC} ${PROTO_SOURCES})
target_include_directories(ppc2cpp_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
  ${PROTO_DEST}
)
target_link_libraries(ppc2cpp_core PUBLIC dolrel ppcdisasm protobuf::libprotobuf elfio)

add_executable(project_demo test/program_loader_demo.cpp)
target_include_directories(project_demo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(project_demo ppc2cpp_core)

###### TESTS
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