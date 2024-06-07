//
// Created by 203 on 2021/8/31.
//

#ifndef ANDROID_PRACTICE_TRIANGLESAMPLE_H
#define ANDROID_PRACTICE_TRIANGLESAMPLE_H


#include "../util/ImageDef.h"
#include "GLSampleBase.h"

#define GET_STR(x) #x

class TriangleSample : public GLSampleBase {
public:
    TriangleSample();

    virtual ~TriangleSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH);

    void UpdateModel(glm::mat4 &model);

protected:
    glm::mat4  model;
    GLint m_ModelMatLoc;
    GLint m_LightColorLoc;


    // virtual void OnDraw();
};


#endif //ANDROID_PRACTICE_TRIANGLESAMPLE_H
