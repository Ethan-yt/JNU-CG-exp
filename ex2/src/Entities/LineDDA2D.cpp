//
// Created by Ethan on 2017/11/16.
//

#include "LineDDA2D.h"

void LineDDA2D::drawMe() {
    glBegin(GL_POINTS);

    int x1 = sP.x, x2 = eP.x, y1 = sP.y, y2 = eP.y;

    int dm = 0;
    if (abs(x2 - x1) > abs(y2 - y1))
        dm = abs(x2 - x1);            // x 为记长方向
    else
        dm = abs(y2 - y1);            // y 为记长方向

    float dx = (float)(x2 - x1) / dm; // 当x为记长方向时，dx的值为1
    float dy = (float)(y2 - y1) / dm; // 当y为记长方向时，dy的值为1
    float x = x1 + 0.5f;
    float y = y1 + 0.5f;
    for (int i = 0; i < dm; i++) {
        glVertex2i((int) x, (int) y);
        x += dx;
        y += dy;
    }

    glEnd();
}
