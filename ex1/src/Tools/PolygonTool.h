//
// Created by Ethan on 2017/10/26.
//

#ifndef EX1_POLYGONTOOL_H
#define EX1_POLYGONTOOL_H


#include "Tool.h"

class PolygonTool : public Tool {
private:
    bool first;
public:

    explicit PolygonTool() : Tool( "Polygon") {
        first = true;
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

};


#endif //EX1_POLYGONTOOL_H
