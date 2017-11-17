//
// Created by Ethan on 2017/10/25.
//


#include "App.h"

#include "Tools/Tool.h"
#include "Tools/LineTool.h"
#include "Tools/PolygonTool.h"
#include "Tools/CircleTool.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
App *App::instance = nullptr;

void App::renderScene() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto &entity :entities) {
        entity->drawMe();
    }
    glutSwapBuffers();
}

void App::resizeScene(int width, int height) {
    //--------------reset the viewport--------------
    // avoid divide by zero
    if (height == 0) {
        height = 1;
    }
    view_width = width;
    view_height = height;

    // reset the viewport to the new dimensions
    glViewport(0, 0, width, height);

}

void App::changeTool(Tool *tool) {
    delete currentTool;
    currentTool = tool;

    char str[100];
    sprintf(str, "%s [%s]", title, tool->name);

    glutSetWindowTitle(str);
}


// static functions
void App::sRenderScene() {
    instance->renderScene();
}

void App::sResizeScene(int width, int height) {
    instance->resizeScene(width, height);
}

void App::sMouseFunc(int button, int state, int x, int y) {
    instance->currentTool->mouseFunc(button, state, x, y);

}

void App::sPassiveMotionFunc(int x, int y) {
    instance->currentTool->passiveMotionFunc(x, y);
}

void App::sMotionFunc(int x, int y) {
    instance->currentTool->motionFunc(x, y);
}

void App::createGlutMenus() {
    glutCreateMenu(sMenuEvent);
    glutAddMenuEntry("Line", LINE);
    glutAddMenuEntry("Polygon", POLYGON);
    glutAddMenuEntry("Circle", CIRCLE);
    glutAddMenuEntry("Clear", CLEAR);
    glutAddMenuEntry("------------", SPACE);
    glutAddMenuEntry("Exit", QUIT);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

}

void App::sMenuEvent(int option) {
    switch (option) {
        case LINE:
            getInstance()->changeTool(new LineTool());
            break;
        case POLYGON:
            getInstance()->changeTool(new PolygonTool());
            break;
        case CIRCLE:
            getInstance()->changeTool(new CircleTool());
            break;
        case CLEAR:
            getInstance()->clearEntities();
            break;
        case QUIT:
            exit(0);
        default:
            break;
    }
}

void App::init(int width, int height, const char *title) {
    delete instance;
    instance = new App(width, height, title);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(500, 500);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

    gluOrtho2D(instance->left, instance->right, instance->bottom, instance->top);

    glutReshapeFunc(sResizeScene);
    glutDisplayFunc(sRenderScene);
    glutMouseFunc(sMouseFunc);
    glutPassiveMotionFunc(sPassiveMotionFunc);
    glutMotionFunc(sMotionFunc);

    instance->createGlutMenus();

    instance->changeTool(new LineTool());

    glutMainLoop();
}

// static functions end

void App::clearEntities() {
    auto tmp = entities;
    entities.clear();

    for (auto &entity : tmp) {
        delete entity;
    }
    glutPostRedisplay();
}

Point2D App::convertPoint(Point2D p) {
    p.y = view_height - p.y;
    float xf = (p.x - (float) view_width / 2) / view_width * (right - left);
    float yf = (p.y - (float) view_height / 2) / view_height * (top - bottom);
    return {xf, yf};
}

#pragma clang diagnostic pop