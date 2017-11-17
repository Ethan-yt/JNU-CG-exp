//
// Created by Ethan on 2017/10/26.
//

#include "PolygonTool.h"
#include "../Entities/Polygon2D.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void PolygonTool::passiveMotionFunc(int x, int y) {
    if (!first) {
        Point2D p = app->convertPoint({x, y});
        auto &points = ((Polygon2D *) entity)->points;
        points.back() = p;
        glutPostRedisplay();
    }
}

void PolygonTool::mouseFunc(int button, int state, int x, int y) {

    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        // 左键按下代表开始画图

        Point2D p = app->convertPoint({x, y});

        if (first) {
            entity = new Polygon2D();
            auto &points = ((Polygon2D *) entity)->points;
            points.push_back(p);
            points.push_back(p);

            App::getInstance()->entities.push_back(entity);
            first = !first;
        } else {
            auto &points = ((Polygon2D *) entity)->points;
            points.back() = p;
            points.push_back(p);

        }
        glutPostRedisplay();
    }
}

void PolygonTool::motionFunc(int x, int y) {

}

#pragma clang diagnostic pop