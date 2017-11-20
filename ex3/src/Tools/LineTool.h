//
// Created by Ethan on 2017/10/25.
//

#ifndef EX1_LINETOOL_H
#define EX1_LINETOOL_H


#include "Tool.h"

class LineTool : public Tool {
private:
    bool first;
public:

    LineTool() {
        first = true;
        createEntity();
    }

    void mouseFunc(int button, int state, int x, int y) override;

    void passiveMotionFunc(int x, int y) override;

    void motionFunc(int x, int y) override;

    ~LineTool() override {
        std::cout << "LineTool Deleted" << std::endl;
        if (first) deleteEntity();
    }

    void createEntity();

    void deleteEntity();

};


#endif //EX1_LINETOOL_H
