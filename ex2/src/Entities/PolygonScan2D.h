//
// Created by Ethan on 2017/11/16.
//

#ifndef EX2_POLYGONSCAN2D_H
#define EX2_POLYGONSCAN2D_H


#include <iostream>
#include "Polygon2D.h"

class PolygonScan2D : public Polygon2D {
public:
    static constexpr const char *NAME = "Polygon(Scan)";

    void drawMe() override;

    const char *getName() override { return NAME; };

    ~PolygonScan2D() override {
        points.clear();
        std::cout << NAME << " cleaned" << std::endl;
    }
};


#endif //EX2_POLYGONSCAN2D_H
