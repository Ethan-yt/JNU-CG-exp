//#include <iostream>
//#include <opencv2/opencv.hpp>
#include <opencv/highgui.h> // for cvLoadImage()
#include <opencv/cv.h> // for cvCvtColor()
#include <GLUT/glut.h> // 3D glut
#include "Image.h"
#include <stdio.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
float angle = 0;

Image *texture;

float eye[] = {3, 3, 3, 0};
float center[] = {0, 0, 0};
float up[] = {0, 1, 0};

const GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
const GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat light_position[] = {5.0f, 2.0f, 5.0f, 0.0f};

const GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
const GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
const GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat high_shininess[] = {100.0f};


// 绘制长方体 width，height，depth分别为长方体的长，高和深度
void DrawCube(GLfloat width, GLfloat height, GLfloat depth) {
    GLfloat x = width / 2, y = height / 2, z = depth / 2;

    glBegin(GL_QUADS);
    // 前面(front)
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(-x, -y, z);
    glTexCoord2f(1, 0);
    glVertex3f(x, -y, z);
    glTexCoord2f(1, 1);
    glVertex3f(x, y, z);
    glTexCoord2f(0, 1);
    glVertex3f(-x, y, z);
    // 背面(back)

    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);
    glVertex3f(x, -y, -z);
    glTexCoord2f(1, 0);
    glVertex3f(-x, -y, -z);
    glTexCoord2f(1, 1);
    glVertex3f(-x, y, -z);
    glTexCoord2f(0, 1);
    glVertex3f(x, y, -z);

    // 右侧面(right)
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(x, -y, z);
    glTexCoord2f(1, 0);
    glVertex3f(x, -y, -z);
    glTexCoord2f(1, 1);
    glVertex3f(x, y, -z);
    glTexCoord2f(0, 1);
    glVertex3f(x, y, z);
    // 左侧面(left)
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-x, -y, -z);
    glTexCoord2f(1, 0);
    glVertex3f(-x, -y, z);
    glTexCoord2f(1, 1);
    glVertex3f(-x, y, z);
    glTexCoord2f(0, 1);
    glVertex3f(-x, y, -z);

    // 顶面(top)
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-x, y, z);
    glTexCoord2f(1, 0);
    glVertex3f(x, y, z);
    glTexCoord2f(1, 1);
    glVertex3f(x, y, -z);
    glTexCoord2f(0, 1);
    glVertex3f(-x, y, -z);
    // 底面(bottom)
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);
    glVertex3f(-x, -y, -z);
    glTexCoord2f(1, 0);
    glVertex3f(x, -y, -z);
    glTexCoord2f(1, 1);
    glVertex3f(x, -y, z);
    glTexCoord2f(0, 1);
    glVertex3f(-x, -y, z);
    glEnd();
}

void display() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 0.001, 20);
    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    gluLookAt(eye[0], eye[1], eye[2],
              center[0], center[1], center[2],
              up[0], up[1], up[2]);

    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    DrawCube(1.0f, 2.0f, 2.0f);
    glTranslatef(1,-0.5,0.5);
    DrawCube(1.0f, 1.0f, 1.0f);
    glPopMatrix();
    glFlush();
}

void timer(int t) {
    glutTimerFunc(20, timer, 0); /// 1000 msec 50fps:20msec
    angle += 1;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("3D");

    glutDisplayFunc(display); ///显示
    glutTimerFunc(0, timer, 0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST); ///要启动 Detph Test 深度值的测试,3D显示才正确
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glClearColor(1, 1, 1, 1);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glLineWidth(3);
    texture = new Image("../images/texture.jpeg");
    glutMainLoop();
}

#pragma clang diagnostic pop