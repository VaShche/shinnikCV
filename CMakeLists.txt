project(HackCV)
cmake_minimum_required(VERSION 3.5)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}") 
 

set (OpenCV_STATIC ON)
find_package(OpenCV REQUIRED core )

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )
endif()



add_subdirectory(nested/dlib)
add_subdirectory(src)