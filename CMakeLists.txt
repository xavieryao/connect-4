cmake_minimum_required(VERSION 3.6)
project(Strategy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Judge.cpp
    Judge.h
    Point.h
    Strategy.cpp
    Strategy.h)

add_library(Strategy SHARED ${SOURCE_FILES})