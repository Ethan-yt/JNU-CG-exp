aux_source_directory(. DIR_ENTITIES_SRCS)
add_library(Entities ${DIR_ENTITIES_SRCS} Line2D.h LineDDA2D.cpp LineDDA2D.h LineMidP2D.cpp LineMidP2D.h Polygon2D.h PolygonScan2D.cpp PolygonScan2D.h)