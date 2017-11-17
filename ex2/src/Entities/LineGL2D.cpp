//
// Created by Ethan on 2017/10/24.
//

#include "LineGL2D.h"

void LineGL2D::drawMe() {
    glBegin(GL_LINES);
    {
        glVertex2f(sP.x, sP.y);
        glVertex2f(eP.x, eP.y);
    }
    glEnd();
}