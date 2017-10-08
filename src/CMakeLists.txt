project(HackCV)
cmake_minimum_required(VERSION 3.5)

include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(Shinik main.cpp 
	detectorRoadSign.hpp detectorRoadSign.cpp 
	classificatorRoadSign.hpp classificatorRoadSign.cpp )
target_link_libraries(Shinik ${OpenCV_LIBS} dlib Cntk.Core-2.2)