﻿//! \file mgcmddraw.h
//! \brief 定义绘图命令基类 MgCommandDraw 和 MgCmdBaseLines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_H_
#define TOUCHVG_CMD_DRAW_H_

#include "mgcmd.h"

//! 绘图命令基类
/*! \ingroup CORE_COMMAND
 */
class MgCommandDraw : public MgCommand
{
public:
    MgCommandDraw(const char* name);
    virtual ~MgCommandDraw();
    
    MgShape* addShape(const MgMotion* sender, MgShape* shape = NULL);
    
    bool touchBeganStep(const MgMotion* sender);
    bool touchMovedStep(const MgMotion* sender);
    bool touchEndedStep(const MgMotion* sender);

    //! 返回新图形的类型，供其他语言重载使用
    virtual int getShapeType() { return m_shape ? m_shape->shapec()->getType() : 0; }
    
    //! 返回当前捕捉结果类型, >=kMgSnapGrid
    int getSnappedType(const MgMotion* sender) const;
    
protected:
    bool _initialize(int shapeType, const MgMotion* sender, MgStorage* s);
    bool _click(const MgMotion* sender);
    virtual int getMaxStep() { return 3; }
    virtual void setStepPoint(int step, const Point2d& pt);

private:
    virtual bool isDrawingCommand() { return true; }
    
public:
    virtual bool initialize(const MgMotion* sender, MgStorage* s) {
        return _initialize(getShapeType(), sender, s); }
    virtual bool backStep(const MgMotion* sender);
    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool click(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender);
#ifndef SWIG
    virtual const MgShape* getShape(const MgMotion* sender) { return m_shape; }
#endif
    int getStep() { return m_step; }
    MgShape* dynshape() { return m_shape; }
    void setStep(int step) { m_step = step; }
    Point2d snapPoint(const MgMotion* sender, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, bool firstStep, int handle);
    
protected:
    int         m_step;
private:
    MgShape*    m_shape;
    bool        m_oneShapeEnd;
};

#endif // TOUCHVG_CMD_DRAW_H_
