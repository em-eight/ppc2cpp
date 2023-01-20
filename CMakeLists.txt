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

file(GLOB ninloader_SRC
     "src/program_loader/*.cpp"
)
add_library(ninloader ${ninloader_SRC})
target_include_directories(ninloader PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(ninloader dolrel)

add_executable(ploader_demo src/demo/program_loader_demo.cpp)
target_include_directories(ploader_demo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
target_link_libraries(ploader_demo ninloader)