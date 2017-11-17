//
// Created by Ethan on 2017/10/26.
//

#include "PolygonGL2D.h"

void PolygonGL2D::drawMe() {
    glBegin(GL_POLYGON);
    {
        for (auto &point : points)
            glVertex2f(point.x, point.y);
        glVertex2f(points.front().x, points.front().y);
    }
    glEnd();

}
