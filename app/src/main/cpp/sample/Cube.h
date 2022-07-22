//
// Created by shixin on 2022/7/22.
//

#ifndef NDK_PRACTICE_CUBE_H
#define NDK_PRACTICE_CUBE_H


#include "GLSampleBase.h"

class Cube : public GLSampleBase{
public:
    Cube();

    virtual ~Cube();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH);
};


#endif //NDK_PRACTICE_CUBE_H
