//
// Created by Ethan on 2017/10/19.
//

#ifndef EX1_ENTITY2D_H
#define EX1_ENTITY2D_H

#ifdef __APPLE__

#include <GLUT/glut.h>

#else
#include <GL/glut.h>
#endif

class Entity2D {
public:
    virtual void drawMe()= 0;

    virtual ~Entity2D() = default;
};


#endif //EX1_ENTITY2D_H
