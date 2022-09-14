cmake_minimum_required(VERSION 3.23)
project(apt_integrity)

set(CMAKE_CXX_STANDARD 20)

add_executable(apt_integrity src/main.cpp src/apt_integrity.cpp src/apt_integrity.h)
