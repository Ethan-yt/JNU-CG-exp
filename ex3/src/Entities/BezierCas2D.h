//
// Created by Ethan on 2017/11/20.
//

#ifndef EX3_BEZIERCAS2D_H
#define EX3_BEZIERCAS2D_H


#include "Point2D.h"
#include "Entity2D.h"

class BezierCas2D : public Entity2D {
public:
    static constexpr const char *NAME = "Bezier Curve(de Casteljau)";
    static const int n = 3;

    Point2D p[n + 1];

    int top = 0;

    void drawMe() override;

    const char *getName() override { return NAME; };

};


#endif //EX3_BEZIERCAS2D_H
