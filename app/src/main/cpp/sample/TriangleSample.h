//
// Created by 203 on 2021/8/31.
//

#ifndef ANDROID_PRACTICE_TRIANGLESAMPLE_H
#define ANDROID_PRACTICE_TRIANGLESAMPLE_H


#include "../util/ImageDef.h"
#include "GLSampleBase.h"

class TriangleSample : public GLSampleBase {
public:
    TriangleSample();

    virtual ~TriangleSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    //
    virtual void Destroy();

    virtual void OnDraw();
};


#endif //ANDROID_PRACTICE_TRIANGLESAMPLE_H
