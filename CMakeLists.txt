cmake_minimum_required(VERSION 3.16)
project(5)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(5
        bp.cpp
        bp.hpp
        databases.cpp
        databases.hpp
        EmitAndShit.cpp
        EmitAndShit.h
        hw3_output.cpp
        hw3_output.hpp
        types.cpp
        types.h)
