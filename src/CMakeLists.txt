project(HackCV)
cmake_minimum_required(VERSION 3.5)

include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(Shinik main.cpp )
target_link_libraries(Shinik ${OpenCV_LIBS})