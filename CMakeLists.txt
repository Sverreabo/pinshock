cmake_minimum_required(VERSION 3.28)
project(pinshock LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(pinshock
    pinshock.cpp

    pingame/pingame.hpp
    pingame/pingame.cpp

    pingame/bitboard.hpp
    pingame/bitboard.cpp

    bitset/bitset.hpp
    bitset/bitset.cpp
)

target_compile_features(pinshock PRIVATE cxx_std_23)
