//
// Created by Ethan on 2017/11/16.
//

#include <math.h>
#include "LineMidP2D.h"

#define swap(x, y) {int tmp; tmp = x;x = y; y = tmp;}

void LineMidP2D::drawMe() {
    glBegin(GL_POINTS);

    int x0 = sP.x, x1 = eP.x, y0 = sP.y, y1 = eP.y;
    int a, b, d, x, y, tag = 0;
    if (abs(x1 - x0) < abs(y1 - y0)) {
        swap(x0, y0);
        swap(x1, y1);
        tag = 1;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    a = y0 - y1;
    b = x1 - x0;
    d = a + b / 2;
    if (y0 < y1) {
        x = x0;
        y = y0;
        while (x < x1) {
            if (d < 0) {
                x++;
                y++;
                d = d + a + b;
            } else {
                x++;
                d += a;
            }
            if (tag) glVertex2i(y, x);
            else glVertex2i(x, y);
        }
    } else {
        x = x1;
        y = y1;
        while (x > x0) {
            if (d < 0) {
                x--;
                y++;
                d = d - a + b;
            } else {
                x--;
                d -= a;
            }
            if (tag) glVertex2i(y, x);
            else glVertex2i(x, y);
        }

    }

    glEnd();
}


