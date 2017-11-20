//
// Created by Ethan on 2017/11/20.
//

#ifndef EX2_LINECLIPLB_H
#define EX2_LINECLIPLB_H


#include "Entity2D.h"
#include "Point2D.h"

class LineClipLB2D : public Entity2D {
public:
    Point2D sP, eP;
    static constexpr const char *NAME = "Line Clip(LB)";

    void drawMe() override;

    const char *getName() override { return NAME; };
private:
    int clipTest(float p, float q, float &u1, float &u2);
};


#endif //EX2_LINECLIPLB_H
