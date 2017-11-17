//
// Created by Ethan on 2017/10/30.
//

#include "Circle2D.h"
#include <cmath>

void Circle2D::drawMe() {
    glBegin(GL_POLYGON);
    {
        for (int i = 0; i < n; i++) {
            float x = centre.x + radius * cosf(2 * PI * i / n);
            float y = centre.y + radius * sinf(2 * PI * i / n);
            glVertex2f(x, y);
        }
    }
    glEnd();
}
