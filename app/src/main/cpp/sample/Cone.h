//
// Created by shixin on 2022/9/20.
//

#ifndef NDK_PRACTICE_CONE_H
#define NDK_PRACTICE_CONE_H

#include "GLSampleBase.h"
#include <iostream>
#include <math.h>
#include <list>
#include "../util/LogUtil.h"
#include "../util/GLUtils.h"


using namespace std;

class Cone : public GLSampleBase {
public:
    Cone();

    virtual ~Cone();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH);

public:
    GLfloat *gLfloat;
    int vSize;
};


#endif //NDK_PRACTICE_CONE_H
