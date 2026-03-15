# Gather all test source files (same patterns as the legacy Makefile)
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*_tests.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/*_tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp
)

add_executable(runtests
    ${CMAKE_CURRENT_SOURCE_DIR}/doctest.cc
    ${TEST_SOURCES}
)

target_include_directories(runtests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(runtests PRIVATE cpputil::cpputil)

target_compile_options(runtests PRIVATE
    -Wall
    -Wextra
    -Wdeprecated-enum-float-conversion
    -Wenum-compare
    -Wno-unused-const-variable
)

target_compile_definitions(runtests PRIVATE TESTPROJECT)

set_target_properties(runtests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

enable_testing()
add_test(NAME unit_tests COMMAND runtests --no-intro)

option(CPPUTIL_COVERAGE "Enable coverage instrumentation" OFF)
if(CPPUTIL_COVERAGE)
    target_compile_options(cpputil PRIVATE --coverage -O0)
    target_compile_options(runtests PRIVATE --coverage -O0)
    target_link_options(runtests PRIVATE --coverage)
endif()
