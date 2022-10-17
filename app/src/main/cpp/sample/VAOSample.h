//
// Created by shixin on 2022/10/17.
//

#ifndef NDK_PRACTICE_VAOSAMPLE_H
#define NDK_PRACTICE_VAOSAMPLE_H

#include "GLSampleBase.h"

class VAOSample: public GLSampleBase {
public:
    VAOSample();
    ~VAOSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix,int screenW, int screenH);




};


#endif //NDK_PRACTICE_VAOSAMPLE_H
