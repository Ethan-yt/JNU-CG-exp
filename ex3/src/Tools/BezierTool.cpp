//
// Created by Ethan on 2017/11/20.
//

#include <Entities/BezierCas2D.h>
#include "BezierTool.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void BezierTool::mouseFunc(int button, int state, int x, int y) {
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        Point2D p = app->convertPoint({x, y});
        (entity)->p[entity->top] = p;
        if (entity->top != entity->n) {
            entity->top++;
            (entity)->p[entity->top] = p;
        } else
            createEntity();

        glutPostRedisplay();
    }
}

void BezierTool::passiveMotionFunc(int x, int y) {
    if (entity->top != 0) {
        Point2D p = app->convertPoint({x, y});
        entity->p[entity->top] = p;
        glutPostRedisplay();
    }

}

void BezierTool::motionFunc(int x, int y) {

}

void BezierTool::createEntity() {
    Tool::entity = new BezierCas2D;
    entity = (BezierCas2D *) Tool::entity;
    App::getInstance()->entities.push_back(Tool::entity);

}

void BezierTool::deleteEntity() {
    auto &entities = App::getInstance()->entities;
    for (auto it = entities.end(); it != entities.begin(); it--)
        if (*it == entity) {
            it = entities.erase(it);
            break;
        }
    delete entity;
}

#pragma clang diagnostic pop