cmake_minimum_required(VERSION 3.28)
project(My-LSM-Tree LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

# additional warnings
add_compile_options(-Wall -Wextra -Wpedantic)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -fomit-frame-pointer -ftree-vectorize -flto -mavx2 -mbmi2 -mpopcnt -mlzcnt -mfma -DNDEBUG")


message(CMAKE_CXX_FLAGS_RELEASE = ${CMAKE_CXX_FLAGS_RELEASE})

SET(SOURCES
    src/main.cpp
    src/tests.h
    src/common/geom.h
    src/r_tree/r_tree.h)

add_executable(main ${SOURCES})

