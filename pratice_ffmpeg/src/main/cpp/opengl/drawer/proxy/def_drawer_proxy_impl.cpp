//
// Created by shixin on 2022/10/31.
//

#include "def_drawer_proxy_impl.h"


void DefDrawerProxyImpl::AddDrawer(Drawer *drawer) {
    m_drawers.push_back(drawer);
}

void DefDrawerProxyImpl::Draw() {
    for (const auto &item: m_drawers) {
        item->Draw();
    }
}

void DefDrawerProxyImpl::Release() {
    for (const auto &item: m_drawers) {
        delete item;
    }
    m_drawers.clear();
}
