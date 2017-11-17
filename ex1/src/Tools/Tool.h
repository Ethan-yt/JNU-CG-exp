//
// Created by Ethan on 2017/10/24.
//

#ifndef EX1_RUBBLETOOL_H
#define EX1_RUBBLETOOL_H

#include "../App.h"

class Tool {
protected:
    Entity2D *entity;
    App *app = App::getInstance();
public:
    const char *name;

    explicit Tool(const char *name) {
        this->name = name;
    }

    virtual void mouseFunc(int button, int state, int x, int y) = 0;

    virtual void passiveMotionFunc(int x, int y) = 0;

    virtual void motionFunc(int x, int y) = 0;

    virtual ~Tool() = default;

};


#endif //EX1_RUBBLETOOL_H
