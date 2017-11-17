//
// Created by Ethan on 2017/11/16.
//

#ifndef EX2_POLYGON2D_H
#define EX2_POLYGON2D_H


#include <vector>
#include "Point2D.h"
#include "Entity2D.h"

class Polygon2D : public Entity2D {
public:
    std::vector<Point2D> points;
};


#endif //EX2_POLYGON2D_H
