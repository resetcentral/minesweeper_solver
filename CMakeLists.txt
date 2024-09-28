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

include(code-coverage.cmake)
add_code_coverage()

add_library(
  minesweeper
  STATIC
  include/minesweeper.hpp
  lib/minesweeper.cpp
)
target_include_directories(minesweeper PUBLIC include/)

add_library(
  node
  include/solver/node.hpp
  lib/solver/node.cpp
)
target_link_libraries(
  node
  minesweeper
  Boost::headers
)

add_library(
  sle
  include/solver/sle.hpp
  lib/solver/sle.cpp
)
target_link_libraries(
  sle
  Boost::headers
)
target_include_directories(sle PUBLIC include/)

add_library(
  solver
  include/solver/solver.hpp
  lib/solver/solver.cpp
)
target_link_libraries(
  solver
  node
  sle
  Boost::headers
)

add_executable(
  runner
  src/runner.cpp
)
target_link_libraries(
  runner
  solver
)

# Tests
add_executable(
  generator_test
  src/tests/generator.cpp
)
target_link_libraries(
  generator_test
  minesweeper
  GTest::gtest_main
)

add_executable(
  minesweeper_test
  src/tests/minesweeper.cpp
)
target_link_libraries(
  minesweeper_test
  minesweeper
  GTest::gtest_main
)

add_executable(
  solver_sle_test
  src/tests/solver/sle.cpp
)
target_link_libraries(
  solver_sle_test
  sle
  node
  GTest::gtest_main
  gmock_main
)

add_executable(
  solver_node_test
  src/tests/solver/node.cpp
)
target_link_libraries(
  solver_node_test
  node
  GTest::gtest_main
  gmock_main
)

add_executable(
  solver_state_test
  src/tests/solver/solver_state.cpp
)
target_link_libraries(
  solver_state_test
  solver
  GTest::gtest_main
  gmock_main
)

add_executable(
  solver_test
  src/tests/solver/solver.cpp
)
target_link_libraries(
  solver_test
  solver
  GTest::gtest_main
  gmock_main
)

include(GoogleTest)
gtest_discover_tests(generator_test)
gtest_discover_tests(minesweeper_test)
gtest_discover_tests(solver_sle_test)
gtest_discover_tests(solver_node_test)
gtest_discover_tests(solver_state_test)
gtest_discover_tests(solver_test)

target_code_coverage(generator_test)
target_code_coverage(minesweeper_test)
target_code_coverage(solver_sle_test)
target_code_coverage(solver_node_test)
target_code_coverage(solver_state_test)
target_code_coverage(solver_test)
add_code_coverage_all_targets()