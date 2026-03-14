### cpputil ###

A collection of utility and helpers in C++. Written for c++20, but most headers will work with c++17 or earlier.

Generally, these were written for embedded projects that lacked the standard library.

Tested with g++ 11 and newer, and clang++ 15 and newer.

To use, include the `cpputil` directory with your build system, and then #include what you use. 

To use the lookup tables, you need to compile `math_tables.cc` as well.

You can also build `int_to_str.cc` if you want that functionality.

### Integrating with cmake

This is mostly a header-only include-what-you-want library, but if you want to integrate this
into another cmake project you can simply add this to your upstream CMakeLists.txt:

```cmake
add_subdirectory(path/to/cpputil)
```

### Tests

To build and run tests:

```bash
cmake --fresh -B build -DCPPUTIL_BUILD_TESTS=ON
cmake --build build
```

