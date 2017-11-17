//
// Created by Ethan on 2017/10/26.
//

#ifndef EX1_POLYGON2D_H
#define EX1_POLYGON2D_H


#include <iostream>
#include <vector>
#include "Entity2D.h"
#include "Point2D.h"

class Polygon2D : public Entity2D {
public:
    void drawMe() override;

    ~Polygon2D() override {
        points.clear();
        std::cout << "Polygon cleaned" << std::endl;
    }
    std::vector<Point2D> points;

};


#endif //EX1_POLYGON2D_H
