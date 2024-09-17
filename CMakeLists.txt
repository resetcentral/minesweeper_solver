cmake_minimum_required(VERSION 3.18)
project(minesweeper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "Debug")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

find_package(Boost
  1.86.0
  COMPONENTS headers)

enable_testing()

add_library(
  minesweeper
  STATIC
  include/minesweeper.hpp
  lib/minesweeper.cpp
)
target_include_directories(minesweeper PUBLIC include/)

target_compile_options(
  minesweeper
  PRIVATE
  "-fprofile-instr-generate"
  "-fcoverage-mapping"
)

add_executable(
  generator
  src/tests/generator.cpp
)
target_include_directories(generator PUBLIC include/)

target_compile_options(
  generator
  PRIVATE
  "-fprofile-instr-generate"
  "-fcoverage-mapping"
)

target_link_libraries(
  generator
  minesweeper
  GTest::gtest_main
  "-fprofile-instr-generate"
  "-fcoverage-mapping"
)

add_executable(
  minesweeper_test
  src/tests/minesweeper.cpp
)
target_include_directories(minesweeper_test PUBLIC include/)

target_compile_options(
  minesweeper_test
  PRIVATE
  "-fprofile-instr-generate"
  "-fcoverage-mapping"
)

target_link_libraries(
  minesweeper_test
  minesweeper
  GTest::gtest_main
  "-fprofile-instr-generate"
  "-fcoverage-mapping"
)

add_executable(
  solver
  src/solver.cpp
)
target_include_directories(generator PUBLIC include/)
target_link_libraries(
  solver
  minesweeper
  Boost::headers
)

include(GoogleTest)
gtest_discover_tests(generator)
gtest_discover_tests(minesweeper_test)