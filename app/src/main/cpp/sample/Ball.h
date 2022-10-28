//
// Created by shixin on 2022/10/28.
//

#ifndef NDK_PRACTICE_BALL_H
#define NDK_PRACTICE_BALL_H

#include "GLSampleBase.h"
#include <iostream>
#include <math.h>
#include <list>
#include "../glm/gtc/matrix_transform.hpp"


class Ball : public GLSampleBase {

public:
    Ball();

    virtual  ~Ball();

    virtual void LoadImage(NativeImage *PImage);


    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH);


public:
    GLfloat *gLfloat{};
    int vSize{};
};


#endif //NDK_PRACTICE_BALL_H
