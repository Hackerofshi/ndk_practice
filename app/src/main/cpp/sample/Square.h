//
// Created by shixin on 2022/7/21.
//

#ifndef NDK_PRACTICE_SQUARE_H
#define NDK_PRACTICE_SQUARE_H

#include "GLSampleBase.h"

class Square : public GLSampleBase{
public:
    Square();

    virtual ~Square();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix,int screenW, int screenH);
};


#endif //NDK_PRACTICE_SQUARE_H
