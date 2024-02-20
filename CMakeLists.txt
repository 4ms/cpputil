project(cpputil CXX)

add_library(cpputil)

target_include_directories(cpputil PUBLIC .)

target_sources(cpputil PRIVATE util/math_tables.cc)

