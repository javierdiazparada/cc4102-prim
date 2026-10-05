cmake_minimum_required(VERSION 3.16)
project(prim_mst_experiments LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(prim_core
  src/graph.cpp
  src/binomial_heap.cpp
  src/fibonacci_heap.cpp
  src/prim.cpp
  src/experiments.cpp
)
target_include_directories(prim_core PUBLIC include)

function(prim_target_options target_name)
  target_compile_options(${target_name} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
  )
endfunction()

prim_target_options(prim_core)

add_executable(prim_mst src/main.cpp)
target_link_libraries(prim_mst PRIVATE prim_core)
prim_target_options(prim_mst)

add_executable(prim_tests tests/test_main.cpp)
target_link_libraries(prim_tests PRIVATE prim_core)
prim_target_options(prim_tests)

enable_testing()
add_test(NAME prim_tests COMMAND prim_tests)
