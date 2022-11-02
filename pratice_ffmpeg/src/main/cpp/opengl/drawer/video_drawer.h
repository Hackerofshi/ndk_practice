//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_VIDEO_DRAWER_H
#define NDK_PRACTICE_VIDEO_DRAWER_H

#include "drawer.h"
#include "../../render/video_render.h"

class VideoDrawer : public Drawer, public VideoRender {
public:

    VideoDrawer();

    ~VideoDrawer();

    void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) override;

    void Render(OneFrame *on_frame) override;

    void ReleaseRender() override;

    const char *GetVertexShader() override;

    const char *GetFragmentShader() override;

    void InitCstShaderHandler() override;
    void BindTexture() override;
    void PrepareDraw() override;
    void DoneDraw() override;


};


#endif //NDK_PRACTICE_VIDEO_DRAWER_H
