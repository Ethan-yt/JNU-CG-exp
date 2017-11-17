//
// Created by Ethan on 2017/10/19.
//

#ifndef EX1_POINT2D_H
#define EX1_POINT2D_H


class Point2D {
public :
    float x = 0, y = 0;

    Point2D() = default;

    Point2D(int x, int y) : x(x), y(y) {}

    Point2D(float x, float y) : x(x), y(y) {}

};


#endif //EX1_POINT2D_H
