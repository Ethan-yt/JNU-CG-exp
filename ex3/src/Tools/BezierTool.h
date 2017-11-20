//
// Created by Ethan on 2017/11/20.
//

#ifndef EX3_BEZIERTOOL_H
#define EX3_BEZIERTOOL_H


#include "Tool.h"

class BezierTool : public Tool {
private:
    BezierCas2D *entity;
public:
    BezierTool() {
        createEntity();
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

    ~BezierTool() {
        std::cout << "BezierTool Deleted" << std::endl;
        if ((entity)->top == 0) deleteEntity();
    }

    void createEntity();

    void deleteEntity();
};


#endif //EX3_BEZIERTOOL_H
