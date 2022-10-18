//
// Created by shixin on 2022/10/18.
//

#ifndef NDK_PRACTICE_TEXTUREMAPSAMPLE3_H
#define NDK_PRACTICE_TEXTUREMAPSAMPLE3_H

#include <GLES3/gl3.h>
#include "../util/ImageDef.h"
#include "GLSampleBase.h"

class TextureMapSample3 : public GLSampleBase {
public:
    TextureMapSample3();

    virtual ~TextureMapSample3();

    void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
};


#endif //NDK_PRACTICE_TEXTUREMAPSAMPLE3_H
