cmake_minimum_required(VERSION 3.9)
project(ex5)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework GLUT -framework OpenGL")

aux_source_directory(. SOURCE_FILES)

add_executable(ex5 ${SOURCE_FILES})

target_link_libraries(ex5 ${OpenCV_LIBS})