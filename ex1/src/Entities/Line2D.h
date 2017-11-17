//
// Created by Ethan on 2017/10/24.
//

#ifndef EX1_LINE2D_H
#define EX1_LINE2D_H


#include <iostream>
#include "Entity2D.h"
#include "Point2D.h"

class Line2D : public Entity2D {
public:
    void drawMe() override;

    ~Line2D() override {
        std::cout << "Line cleaned" << std::endl;
    }

    Point2D sP, eP;
};


#endif //EX1_LINE2D_H
