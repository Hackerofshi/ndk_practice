//
// Created by shixin on 2022/11/11.
//

#ifndef NDK_PRACTICE_BASICLIGHTING_H
#define NDK_PRACTICE_BASICLIGHTING_H


#include "GLSampleBase.h"
#include <iostream>
#include <math.h>
#include <list>
#include "../util/LogUtil.h"
#include "../util/GLUtils.h"


using namespace std;


class BasicLighting : GLSampleBase {
public:
    BasicLighting();

    virtual ~BasicLighting();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH);

public:
    GLuint m_ProgramObj1;

};


#endif //NDK_PRACTICE_BASICLIGHTING_H
