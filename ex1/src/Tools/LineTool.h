//
// Created by Ethan on 2017/10/25.
//

#ifndef EX1_LINETOOL_H
#define EX1_LINETOOL_H


#include "../Entities/Line2D.h"
#include "Tool.h"

class LineTool : public Tool {
private:
    bool first;
public:

    explicit LineTool() : Tool("Line") {
        first = true;
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

};


#endif //EX1_LINETOOL_H
