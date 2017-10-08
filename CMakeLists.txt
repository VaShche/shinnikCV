project(HackCV)
cmake_minimum_required(VERSION 3.5)

## OpenMP
find_package(OpenMP REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS}") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}") 
 
## OpenCV
set (OpenCV_STATIC ON)
find_package(OpenCV REQUIRED core )

if (MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )
endif()


set(DLIB_NO_GUI_SUPPORT OFF)

option(USE_AVX_INSTRUCTIONS  "Compile your program with AVX instructions" ON)

IF(USE_AVX_INSTRUCTIONS)
  add_definitions(-mavx)
  add_definitions(-march=native)
ENDIF()


## CNTK
include_directories(nested/cntk/Include)
link_directories(nested/cntk/cntk)


include(transito-cv/dlib/dlib/cmake)
#include_directories(transito-cv/dlib)
#add_subdirectory(nested/dlib)
#add_subdirectory(transito-cv) 

add_subdirectory(src)
