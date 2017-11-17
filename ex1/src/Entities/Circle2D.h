//
// Created by Ethan on 2017/10/30.
//

#ifndef EX1_CIRCLE2D_H
#define EX1_CIRCLE2D_H


#include <iostream>
#include "Entity2D.h"
#include "Point2D.h"

class Circle2D : public Entity2D {
public:
    void drawMe() override;

    ~Circle2D() override {
        std::cout << "Circle cleaned" << std::endl;
    }

    Point2D centre;
    float radius;
    float n = 50;
    const float PI=3.1415926f;
};


#endif //EX1_CIRCLE2D_H
