cmake_minimum_required(VERSION 3.23)
project(apt_integrity)

set(CMAKE_CXX_STANDARD 20)

add_executable(apt_integrity src/main.cpp src/apt_integrity.cpp src/apt_integrity.h src/threadsafequeue.cpp src/threadsafequeue.h)

find_package(OpenSSL REQUIRED)
target_link_libraries(apt_integrity OpenSSL::SSL)
