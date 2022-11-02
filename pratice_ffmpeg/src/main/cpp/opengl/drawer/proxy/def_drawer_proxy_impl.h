//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_DEF_DRAWER_PROXY_IMPL_H
#define NDK_PRACTICE_DEF_DRAWER_PROXY_IMPL_H

#include "drawer_proxy.h"
#include <vector>


class DefDrawerProxyImpl : public DrawerProxy {
private:
    std::vector<Drawer *> m_drawers;

public:
    void AddDrawer(Drawer *drawer);

    void Draw() override;

    void Release() override;

    void SetScreenSize(int width, int height) override;

};


#endif //NDK_PRACTICE_DEF_DRAWER_PROXY_IMPL_H
