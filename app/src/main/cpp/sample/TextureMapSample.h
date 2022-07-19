//
// Created by shixin on 2022/7/18.
//

#ifndef NDK_PRACTICE_TEXTUREMAPSAMPLE_H
#define NDK_PRACTICE_TEXTUREMAPSAMPLE_H

#include "GLSampleBase.h"
#include "../util/ImageDef.h"

class TextureMapSample : public GLSampleBase {
public:
    TextureMapSample();

    virtual ~TextureMapSample();

    void loadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
};


#endif //NDK_PRACTICE_TEXTUREMAPSAMPLE_H
