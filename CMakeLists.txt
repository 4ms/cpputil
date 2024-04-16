project(cpputil CXX)

add_library(cpputil)
add_library(cpputil::cpputil ALIAS cpputil)

target_compile_features(cpputil PUBLIC cxx_std_20)

target_include_directories(cpputil PUBLIC .)

target_sources(cpputil PRIVATE
    util/math_tables.cc
    util/int_to_str.cc
)


