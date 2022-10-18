//
// Created by shixin on 2022/10/18.
//

#ifndef NDK_PRACTICE_TEXTUREMAPSAMPLE2_H
#define NDK_PRACTICE_TEXTUREMAPSAMPLE2_H


#include <GLES3/gl3.h>
#include "../util/ImageDef.h"
#include "GLSampleBase.h"

class TextureMapSample2 : public GLSampleBase {
public:
    TextureMapSample2();

    virtual ~TextureMapSample2();

    void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
};


#endif //NDK_PRACTICE_TEXTUREMAPSAMPLE2_H
