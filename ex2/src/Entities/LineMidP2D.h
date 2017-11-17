//
// Created by Ethan on 2017/11/16.
//

#ifndef EX2_LINEMIDP2D_H
#define EX2_LINEMIDP2D_H


#include <iostream>
#include "Line2D.h"

class LineMidP2D : public Line2D {
public:
    static constexpr const char *NAME = "Line(Mid Point)";

    void drawMe() override;

    const char *getName() override { return NAME; };
};


#endif //EX2_LINEMIDP2D_H
