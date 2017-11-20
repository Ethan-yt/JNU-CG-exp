//
// Created by Ethan on 2017/11/20.
//

#include "BezierCas2D.h"

void BezierCas2D::drawMe() {
    glBegin(GL_LINES);
    glColor3f(0.5, 0.5, 0.5);
    for (int i = 0; i < top; i++) {
        glVertex2f(p[i].x, p[i].y);
        glVertex2f(p[i + 1].x, p[i + 1].y);
    }
    glEnd();

    if (top == n) {
        glBegin(GL_POINTS);
        glColor3f(1, 1, 1);
        for (int count = 0; count <= 100; count++) {
            Point2D c[n + 1];
            float t = count / 100.0f;
            float t1 = 1.0f - t;

            for (int i = 0; i <= n; i++) {
                c[i] = p[i];
            }

            for (int r = 1; r <= n; r++) {
                for (int i = 0; i <= n - r; i++) {
                    c[i].x = t1 * c[i].x + t * c[i + 1].x;
                    c[i].y = t1 * c[i].y + t * c[i + 1].y;
                }
            }
            glVertex2f(c[0].x, c[0].y);
        }
        glEnd();
    }
}