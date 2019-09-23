cmake_minimum_required (VERSION 2.6)

project (httpserver)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

find_package(Threads)

add_executable(httpserver src/httpserver.cpp)
target_link_libraries(httpserver pthread)