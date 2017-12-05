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

Image *imgEarth;
Image *imgMoon;
Image *imgSun;

float eye[] = {7, 2, 7};
float center[] = {0, 0, 0};
float up[] = {0, 1, 0};

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

    glPushMatrix();                     // 太阳
    glRotatef(90, 1, 0, 0);             // 自转
    glRotatef(angle, 0, 0, 1);          // 自转
    glBindTexture(GL_TEXTURE_2D, imgSun->id);
    gluQuadricTexture(imgSun->quad, 1);
    gluSphere(imgSun->quad, 1, 30, 30);
    glPopMatrix();
    glRotatef(angle, 0, -1, 0);         // 公转
    glTranslatef(4, 0, 0);              // 公转

    glPushMatrix();                     // 地球
    glRotatef(90, 1, 0, 0);             // 自转
    glRotatef(angle, 0, 0, 1);          // 自转
    glBindTexture(GL_TEXTURE_2D, imgEarth->id);
    gluQuadricTexture(imgEarth->quad, 1);
    gluSphere(imgEarth->quad, 0.3, 30, 30);
    glPopMatrix();
    glRotatef(angle, 0, -1, 0);         // 公转
    glTranslatef(1, 0, 0);              // 公转

    glPushMatrix();                     // 月球
    glRotatef(90, 1, 0, 0);             // 自转
    glRotatef(angle, 0, 0, 1);          // 自转
    glBindTexture(GL_TEXTURE_2D, imgMoon->id);
    gluQuadricTexture(imgMoon->quad, 1);
    gluSphere(imgMoon->quad, 0.15, 30, 30);
    glPopMatrix();


    glPopMatrix();
    glFlush();
}

void timer(int t) {
    glutTimerFunc(20, timer, 0); /// 1000 msec 50fps:20msec
    angle += 1;
    glutPostRedisplay();
}

const GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
const GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat light_position[] = {2.0f, 5.0f, 5.0f, 0.0f};

const GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
const GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
const GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat high_shininess[] = {100.0f};

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
//    glClearColor(0, 0, 0, 1);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);


    imgEarth = new Image("../images/earth.jpg");
    imgSun = new Image("../images/sun.jpeg");
    imgMoon = new Image("../images/moon.jpeg");
    glutMainLoop();
}

#pragma clang diagnostic pop