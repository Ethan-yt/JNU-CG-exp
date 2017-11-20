cmake_minimum_required(VERSION 3.6)
project(ex3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework GLUT -framework OpenGL")

aux_source_directory(. SOURCE_FILES)
include_directories(./ ./Entities ./Tools)
add_subdirectory(Entities)
add_subdirectory(Tools)
add_executable(ex3 ${SOURCE_FILES})
target_link_libraries(ex3 Entities Tools)