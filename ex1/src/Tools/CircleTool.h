//
// Created by Ethan on 2017/10/30.
//

#ifndef EX1_CIRCLETOOL_H
#define EX1_CIRCLETOOL_H


#include "Tool.h"

class CircleTool : public Tool {
private:
    bool first;

    float setRadius(float mx, float my);

public:

    explicit CircleTool() : Tool("Circle") {
        first = true;
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

};


#endif //EX1_CIRCLETOOL_H
