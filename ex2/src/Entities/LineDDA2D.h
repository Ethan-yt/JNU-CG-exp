//
// Created by Ethan on 2017/11/16.
//

#ifndef EX2_LINEDDA_H
#define EX2_LINEDDA_H


#include <iostream>
#include "Line2D.h"

class LineDDA2D : public Line2D {
public:
    static constexpr const char *NAME = "Line(DDA)";

    void drawMe() override;

    const char *getName() override { return NAME; };

};


#endif //EX2_LINEDDA_H
