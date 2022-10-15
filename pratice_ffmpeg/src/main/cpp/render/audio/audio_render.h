//
// Created by shixin on 2022/10/14.
//

#ifndef NDK_PRACTICE_AUDIO_RENDER_H
#define NDK_PRACTICE_AUDIO_RENDER_H

#include <cstdint>

class AudioRender {
public:
    virtual void InitRender() = 0;

    virtual void Render(uint8_t *pcm, int size) = 0;

    virtual void ReleaseRender() = 0;

    virtual ~AudioRender() {}
};


#endif //NDK_PRACTICE_AUDIO_RENDER_H
