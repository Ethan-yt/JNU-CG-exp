cmake_minimum_required(VERSION 3.6)
project(ex1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -framework GLUT -framework OpenGL")

set(SOURCE_FILES main.cpp Entities/Entity2D.h Entities/Point2D.h Entities/Line2D.cpp Entities/Line2D.h Tools/Tool.h Tools/LineTool.cpp Tools/LineTool.h App.h App.cpp Entities/Polygon2D.cpp Entities/Polygon2D.h Tools/PolygonTool.cpp Tools/PolygonTool.h Entities/Circle2D.cpp Entities/Circle2D.h Tools/CircleTool.cpp Tools/CircleTool.h)
add_executable(ex1 ${SOURCE_FILES})