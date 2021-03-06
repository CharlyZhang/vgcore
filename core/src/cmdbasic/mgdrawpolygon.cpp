// mgdrawpolygon.cpp: 实现多边形绘图命令类 MgCmdDrawPolygon
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawpolygon.h"
#include "mgbasicsps.h"

bool MgCmdDrawPolygon::initialize(const MgMotion* sender, MgStorage* s)
{
    bool ret = _initialize(MgLines::Type(), sender, s);
    if (ret) {
        ((MgBaseLines*)dynshape()->shape())->setClosed(true);
    }
    return ret;
}
