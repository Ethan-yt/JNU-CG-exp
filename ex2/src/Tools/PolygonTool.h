//
// Created by Ethan on 2017/10/26.
//

#ifndef EX1_POLYGONTOOL_H
#define EX1_POLYGONTOOL_H


#include <Entities/PolygonGL2D.h>
#include "Tool.h"

template<class T>
class PolygonTool : public Tool {
private:
    bool first;
public:

    explicit PolygonTool() {
        first = true;
        entity = new T;
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

    ~PolygonTool() {
        if (first) delete entity;
        std::cout << "PolygonTool Deleted" << std::endl;
    }

};

template<class T>
void PolygonTool<T>::passiveMotionFunc(int x, int y) {
    if (!first) {
        Point2D p = app->convertPoint({x, y});
        auto &points = ((PolygonGL2D *) entity)->points;
        points.back() = p;
        glutPostRedisplay();
    }
}

template<class T>
void PolygonTool<T>::mouseFunc(int button, int state, int x, int y) {

    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        // 左键按下代表开始画图

        Point2D p = app->convertPoint({x, y});

        if (first) {
            auto &points = ((PolygonGL2D *) entity)->points;
            points.push_back(p);
            points.push_back(p);

            App::getInstance()->entities.push_back(entity);
            first = !first;
        } else {
            auto &points = ((PolygonGL2D *) entity)->points;
            points.back() = p;
            points.push_back(p);

        }
        glutPostRedisplay();
    }
}

template<class T>
void PolygonTool<T>::motionFunc(int x, int y) {

}

#endif //EX1_POLYGONTOOL_H
