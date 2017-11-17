//
// Created by Ethan on 2017/10/26.
//

#ifndef EX1_POLYGON2D_H
#define EX1_POLYGON2D_H


#include <iostream>
#include "Entity2D.h"
#include "Polygon2D.h"

class PolygonGL2D : public Polygon2D {
public:
    static constexpr const char *NAME = "Polygon(GL)";

    void drawMe() override;

    const char *getName() override { return NAME; };

    ~PolygonGL2D() override {
        points.clear();
        std::cout << NAME << " cleaned" << std::endl;
    }

};


#endif //EX1_POLYGON2D_H
