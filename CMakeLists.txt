project(HackCV)
cmake_minimum_required(VERSION 3.5)

find_package(OpenCV)

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
set(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} /MP" )
endif()
add_subdirectory(nested/dlib)
add_subdirectory(src)