//
// Created by Ethan on 2017/11/20.
//

#include "LineClipLB2D.h"

void LineClipLB2D::drawMe() {
    float xL = -40, xR = 40, yB = -40, yT = 40;
    float x2 = eP.x, x1 = sP.x, y2 = eP.y, y1 = sP.y;
    glBegin(GL_LINES);
    glColor3f(0.5, 0.5, 0.5);
    glVertex2f(-100, yT);
    glVertex2f(100, yT);
    glVertex2f(-100, yB);
    glVertex2f(100, yB);
    glVertex2f(xL, -100);
    glVertex2f(xL, 100);
    glVertex2f(xR, -100);
    glVertex2f(xR, 100);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();

    float u1 = 0, u2 = 1, dx = x2 - x1, dy;
    if (clipTest(-dx, x1 - xL, u1, u2)) {
        if (clipTest(dx, xR - x1, u1, u2)) {
            dy = y2 - y1;
            if (clipTest(-dy, y1 - yB, u1, u2)) {
                if (clipTest(dy, yT - y1, u1, u2)) {
                    if (u2 < 1) {
                        x2 = x1 + u2 * dx;
                        y2 = y1 + u2 * dy;
                    }
                    if (u1 > 0) {
                        x1 = x1 + u1 * dx;
                        y1 = y1 + u1 * dy;
                    }
                    glBegin(GL_LINES);
                    glColor3f(1, 1, 1);
                    glVertex2f(x1, y1);
                    glVertex2f(x2, y2);
                    glEnd();
                }
            }
        }
    }
}

int LineClipLB2D::clipTest(float p, float q, float &u1, float &u2) {
    float r;
    int retVal = 1;
    if (p < 0) {
        r = q / p;
        if (r > u2) retVal = 0;
        else if (r > u1) u1 = r;
    } else if (p > 0) {
        r = q / p;
        if (r < u1) retVal = 0;
        else if (r < u2) u2 = r;
    } else if (q < 0) retVal = 0;
    return retVal;
}
