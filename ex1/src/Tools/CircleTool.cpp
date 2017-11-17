//
// Created by Ethan on 2017/10/30.
//

#include <cmath>
#include "CircleTool.h"
#include "../Entities/Circle2D.h"
#include "../Entities/Line2D.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void CircleTool::mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {

        Point2D p = app->convertPoint({x, y});

        if (first) {
            entity = new Circle2D;
            ((Circle2D *) entity)->centre = p;
            ((Circle2D *) entity)->radius = 1;
            App::getInstance()->entities.push_back(entity);
        } else {
            setRadius(p.x, p.y);
        }
        first = !first;
        glutPostRedisplay();
    }
}

void CircleTool::passiveMotionFunc(int x, int y) {
    if (!first) {
        Point2D p = app->convertPoint({x, y});
        setRadius(p.x, p.y);
        glutPostRedisplay();
    }
}

void CircleTool::motionFunc(int x, int y) {

}

float CircleTool::setRadius(float mx, float my) {
    Circle2D *e = ((Circle2D *) entity);
    float dx = e->centre.x - mx;
    float dy = e->centre.y - my;
    float r = sqrtf(dx * dx + dy * dy);
    e->radius = r;
}

#pragma clang diagnostic pop