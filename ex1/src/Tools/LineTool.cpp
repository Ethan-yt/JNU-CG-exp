//
// Created by Ethan on 2017/10/25.
//

#include "LineTool.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void LineTool::passiveMotionFunc(int x, int y) {
    if (!first) {
        Point2D p = app->convertPoint({x, y});
        ((Line2D *) entity)->eP = p;
        glutPostRedisplay();
    }
}

void LineTool::mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        Point2D p = app->convertPoint({x, y});
        if (first) {
            entity = new Line2D;
            ((Line2D *) entity)->sP = ((Line2D *) entity)->eP = p;
            App::getInstance()->entities.push_back(entity);
        } else {
            ((Line2D *) entity)->eP = p;

        }
        first = !first;
        glutPostRedisplay();
    }
}

void LineTool::motionFunc(int x, int y) {

}

#pragma clang diagnostic pop