cmake_minimum_required(VERSION 3.12)

project(ppc2cpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  ninutils
  GIT_REPOSITORY https://github.com/em-eight/ninutils.git
  GIT_TAG        b877507806070d0393b3f8d89ea242885a0bbae2
)

FetchContent_MakeAvailable(ninutils)

# Proto stuff
find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/program_loader.proto)

file(GLOB PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
foreach(PROTO_FILE ${PROTO_FILES})
  string(REGEX REPLACE "[.]proto$" ".pb.cc" PROTO_SOURCE ${PROTO_FILE})
  set(PROTO_SOURCES ${PROTO_SOURCES} ${PROTO_SOURCE})
endforeach()

set(PROTO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/proto)
set(PROTO_DEST ${CMAKE_CURRENT_SOURCE_DIR}/proto)

add_custom_target(proto_gen
  COMMAND protoc --proto_path=${PROTO_PATH} --cpp_out=${PROTO_DEST} ${PROTO_FILES}
  COMMAND_EXPAND_LISTS
  BYPRODUCTS ${PROTO_SOURCES}
)

file(GLOB_RECURSE ppc2cpp_core_SRC
     "src/*.cpp"
)
add_library(ppc2cpp_core ${ppc2cpp_core_SRC} ${PROTO_SOURCES})
target_include_directories(ppc2cpp_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
  ${PROTO_DEST}
)
target_link_libraries(ppc2cpp_core dolrel ${Protobuf_LIBRARY})
add_dependencies(ppc2cpp_core proto_gen)

add_executable(project_demo test/program_loader_demo.cpp)
target_include_directories(project_demo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(project_demo ppc2cpp_core)