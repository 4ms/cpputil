cmake_minimum_required(VERSION 3.20)
project(cpputil CXX)

option(CPPUTIL_BUILD_TESTS "Build unit tests" OFF)

add_library(cpputil)
add_library(cpputil::cpputil ALIAS cpputil)

target_compile_features(cpputil PUBLIC cxx_std_20)

target_include_directories(cpputil PUBLIC .)

target_sources(cpputil PRIVATE
    util/math_tables.cc
    util/int_to_str.cc
)

if(CPPUTIL_BUILD_TESTS)
    add_subdirectory(tests)
endif()
