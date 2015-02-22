cmake_minimum_required(VERSION 2.8)

project(instantConfig-tests)

enable_testing()

set(GTEST_ROOT GTEST_ROOT-NOTFOUND CACHE PATH "gtest root directory")

if (WIN32)
  if (NOT GTEST_ROOT)
    message (FATAL_ERROR "GTEST_ROOT path not specified. Cannot build tests")
  endif (NOT GTEST_ROOT)
endif (WIN32)

find_package(GTest REQUIRED)

if (NOT GTEST_FOUND) 
  message (FATAL_ERROR "GTest could not be found")
endif (NOT GTEST_FOUND)

include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}
)

add_executable(icfg-static-test build_test_runner.cpp)

if (UNIX)
  set(ADDITIONAL_LIBS "stdc++" "pthread")
endif (UNIX)

target_link_libraries(icfg-static-test ${GTEST_LIBRARIES} ${ADDITIONAL_LIBS})

get_filename_component(COMPILER_BINARY ${CMAKE_CXX_COMPILER} NAME)

if (UNIX 
  OR COMPILER_BINARY STREQUAL "gcc.exe"
  OR COMPILER_BINARY STREQUAL "g++.exe"
  OR COMPILER_BINARY STREQUAL "clang.exe")
  set( IS_UNIX_COMPILER TRUE)
else()
  set( IS_UNIX_COMPILER FALSE)
endif(UNIX 
  OR COMPILER_BINARY STREQUAL "gcc.exe"
  OR COMPILER_BINARY STREQUAL "g++.exe"
  OR COMPILER_BINARY STREQUAL "clang.exe")
  
message("IS_UNIX_COMPILER : ${IS_UNIX_COMPILER} , ${COMPILER_BINARY}")

if (IS_UNIX_COMPILER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
endif (IS_UNIX_COMPILER)

GTEST_ADD_TESTS(icfg-static-test "" build_test_runner.cpp)

include(GenerateTestProject.cmake)


