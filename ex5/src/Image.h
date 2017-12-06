//
// Created by Ethan on 2017/6/12.
//
#include <opencv/highgui.h> ///for cvLoadImage()
#include <opencv/cv.h> ///for cvCvtColor()

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#ifndef INC_05052553_WEEK15_IMAGE_H
#define INC_05052553_WEEK15_IMAGE_H


class Image {
public:
    Image(char *filename);

    GLUquadric *quad;
    GLuint id;
    IplImage *img;
};


#endif //INC_05052553_WEEK15_IMAGE_H
