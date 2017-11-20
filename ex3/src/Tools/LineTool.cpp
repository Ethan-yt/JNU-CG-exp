//
// Created by Ethan on 2017/11/20.
//

#include <Entities/LineClipLB2D.h>
#include "LineTool.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
void LineTool::passiveMotionFunc(int x, int y) {
    if (!first) {
        Point2D p = app->convertPoint({x, y});
        ((LineClipLB2D *) entity)->eP = p;
        glutPostRedisplay();
    }
}


void LineTool::mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        Point2D p = app->convertPoint({x, y});
        if (first) {
            ((LineClipLB2D *) entity)->sP = ((LineClipLB2D *) entity)->eP = p;

        } else {
            ((LineClipLB2D *) entity)->eP = p;
            createEntity();
        }
        first = !first;
        glutPostRedisplay();
    }
}


void LineTool::motionFunc(int x, int y) {

}

void LineTool::createEntity() {
    entity = new LineClipLB2D;
    App::getInstance()->entities.push_back(entity);
}

void LineTool::deleteEntity() {
    auto &entities = App::getInstance()->entities;
    for(auto  it = entities.end() - 1;it >= entities.begin();it--)
        if(*it == entity){
            it = entities.erase(it);
            break;
        }
    delete entity;
}

#pragma clang diagnostic pop