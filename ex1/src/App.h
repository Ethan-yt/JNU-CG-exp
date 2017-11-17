//
// Created by Ethan on 2017/10/25.
//

#ifndef EX1_APP_H
#define EX1_APP_H

#include <cstdlib>
#include <vector>
#include "Entities/Entity2D.h"
#include "Entities/Point2D.h"

#ifdef __APPLE__

#include <GLUT/glut.h>

#else
#include <GL/glut.h>
#endif

class Tool;

class App {
private:
    App(int width, int height, const char *title)
            : view_width(width), view_height(height), title(title) {};

    static App *instance;

    void resizeScene(int width, int height);

    void renderScene();

    void createGlutMenus();

    void changeTool(Tool *tool);

    void clearEntities();

public:
    App(const App &) = delete;

    App &operator=(const App &) = delete;

    static void init(int width, int height, const char *title);

    static App *getInstance() { return instance; }

    // Called once when the application starts and again every time the window is
    // resized by the user.
    static void sResizeScene(int width, int height);

    static void sRenderScene();

    static void sMouseFunc(int button, int state, int x, int y);

    static void sPassiveMotionFunc(int x, int y);

    static void sMotionFunc(int x, int y);

    static void sMenuEvent(int option);

    int view_width = 500, view_height = 500;
    const char *title = "";
    float left = -100, right = 100, bottom = -100, top = 100;
    Tool *currentTool = nullptr;
    std::vector<Entity2D *> entities;
    enum Menu {
        SPACE, CLEAR, LINE, POLYGON, CIRCLE, QUIT
    };

    Point2D convertPoint(Point2D p);
};

#endif  // EX1_APP_H