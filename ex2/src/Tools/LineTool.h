//
// Created by Ethan on 2017/10/25.
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#ifndef EX1_LINETOOL_H
#define EX1_LINETOOL_H


#include "Tool.h"

template<class T>
class LineTool : public Tool {
private:
    bool first;
public:

    LineTool() {
        first = true;
        entity = new T;
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

    ~LineTool() {
        std::cout << "LintTool Deleted" << std::endl;
        if (first) delete entity;
    }

};

template<class T>
void LineTool<T>::passiveMotionFunc(int x, int y) {
    if (!first) {
        Point2D p = app->convertPoint({x, y});
        ((Line2D *) entity)->eP = p;
        glutPostRedisplay();
    }
}

template<class T>
void LineTool<T>::mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        Point2D p = app->convertPoint({x, y});
        if (first) {
            ((Line2D *) entity)->sP = ((Line2D *) entity)->eP = p;
            App::getInstance()->entities.push_back(entity);
        } else {
            ((Line2D *) entity)->eP = p;
            entity = new T;
        }
        first = !first;
        glutPostRedisplay();
    }
}

template<class T>
void LineTool<T>::motionFunc(int x, int y) {

}

#endif //EX1_LINETOOL_H

#pragma clang diagnostic pop