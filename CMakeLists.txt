cmake_minimum_required (VERSION 2.6)

project (httpserver)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(LINKLIBS "pthread")

find_package(Threads)

if (WIN32) 
	set(LINKLIBS "")
endif()

add_executable(httpserver examples/ComplexServerExample.cpp)

target_link_libraries(httpserver ${LINKLIBS})