//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_DRAWER_PROXY_H
#define NDK_PRACTICE_DRAWER_PROXY_H

#include "../drawer.h"

class DrawerProxy {
public:
    virtual void AddDrawer(Drawer *drawer) = 0;

    virtual void Draw() = 0;

    virtual void Release() = 0;

    virtual ~DrawerProxy() {}
};


#endif //NDK_PRACTICE_DRAWER_PROXY_H
