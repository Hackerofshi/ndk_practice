//
// Created by shixin on 2022/10/28.
//

#ifndef NDK_PRACTICE_SKYSPHERE_H
#define NDK_PRACTICE_SKYSPHERE_H

#include "GLSampleBase.h"
#include <iostream>
#include <math.h>
#include <list>
#include "../glm/gtc/matrix_transform.hpp"
#include "../util/LogUtil.h"


class SkySphere : public GLSampleBase{
public:
    SkySphere();

    virtual  ~SkySphere();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH);

    virtual void setMatrix(float *pArray);

public:
    GLfloat *gLfloat{};
    GLfloat *gLTexturefloat{};
    int vSize{};

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;

    glm::mat4 rotationMatrix;

};


#endif //NDK_PRACTICE_SKYSPHERE_H
