//
// Created by Ethan on 2017/10/24.
//

#ifndef EX1_LINE2D_H
#define EX1_LINE2D_H


#include <iostream>
#include "Entity2D.h"
#include "Point2D.h"
#include "Line2D.h"

class LineGL2D : public Line2D {
public:
    static constexpr const char *NAME = "Line(GL)";

    void drawMe() override;

    const char *getName() override { return NAME; };
};


#endif //EX1_LINE2D_H
