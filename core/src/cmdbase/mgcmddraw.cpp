// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmddraw.h"
#include "mgsnap.h"
#include "mgaction.h"
#include "cmdsubject.h"
#include <string.h>
#include "mglog.h"
#include "mgspfactory.h"

MgCommandDraw::MgCommandDraw(const char* name)
    : MgCommand(name), m_step(0), m_shape(NULL)
{
}

MgCommandDraw::~MgCommandDraw()
{
    MgObject::release_pointer(m_shape);
}

bool MgCommandDraw::cancel(const MgMotion* sender)
{
    if (m_step > 0) {
        m_step = 0;
        m_shape->shape()->clear();
        sender->view->getSnap()->clearSnap(sender);
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::_initialize(MgShape* (*creator)(), const MgMotion* sender)
{
    if (!m_shape) {
        m_shape = creator ? creator() : sender->view->getShapeFactory()->createShape(getShapeType());
        if (!m_shape || !m_shape->shape())
            return false;
        m_shape->setParent(sender->view->shapes(), 0);
    }
    sender->view->setNewShapeID(0);
    m_step = 0;
    m_shape->shape()->clear();
    m_shape->setContext(*sender->view->context());
    
    return true;
}

MgShape* MgCommandDraw::addShape(const MgMotion* sender, MgShape* shape)
{
    shape = shape ? shape : m_shape;
    MgShape* newsp = NULL;
    
    if (sender->view->shapeWillAdded(shape)) {
        newsp = sender->view->shapes()->addShape(*shape);
        if (shape == m_shape) {
            m_shape->shape()->clear();
            sender->view->shapeAdded(newsp);
        } else {
            sender->view->getCmdSubject()->onShapeAdded(sender, newsp);
        }
        if (strcmp(getName(), "splines") != 0) {
            sender->view->setNewShapeID(newsp->getID());
        }
    }
    if (m_shape && sender->view->context()) {
        m_shape->setContext(*sender->view->context());
    }
    
    return newsp;
}

bool MgCommandDraw::backStep(const MgMotion* sender)
{
    if (m_step > 1) {
        m_step--;
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::draw(const MgMotion* sender, GiGraphics* gs)
{
    bool ret = m_step > 0 && m_shape->draw(0, *gs, NULL, -1);
    return sender->view->getSnap()->drawSnap(sender, gs) || ret;
}

bool MgCommandDraw::gatherShapes(const MgMotion* /*sender*/, MgShapes* shapes)
{
    if (m_step > 0 && m_shape && m_shape->shapec()->getPointCount() > 0) {
        shapes->addShape(*m_shape);
    }
    return false; // gather more shapes via draw()
}

bool MgCommandDraw::click(const MgMotion* sender)
{
    return (m_step == 0 ? _click(sender)
            : touchBegan(sender) && touchEnded(sender));
}

bool MgCommandDraw::_click(const MgMotion* sender)
{
    Box2d limits(sender->pointM, sender->displayMmToModel(10.f), 0);
    MgHitResult res;
    const MgShape* shape = sender->view->shapes()->hitTest(limits, res);
    
    if (shape) {
        sender->view->setNewShapeID(shape->getID());
        sender->view->toSelectCommand();
        LOGD("Command (%s) cancelled after the shape #%d clicked.", getName(), shape->getID());
    }
    
    return shape || (sender->view->useFinger() && longPress(sender));
}

bool MgCommandDraw::longPress(const MgMotion* sender)
{
    return sender->view->getAction()->showInDrawing(sender, m_shape);
}

bool MgCommandDraw::touchBegan(const MgMotion* sender)
{
    m_shape->setContext(*sender->view->context());
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::touchMoved(const MgMotion* sender)
{
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::touchEnded(const MgMotion* sender)
{
    sender->view->getSnap()->clearSnap(sender);
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::mouseHover(const MgMotion* sender)
{
    return m_step > 0 && touchMoved(sender);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, bool firstStep)
{
    return snapPoint(sender, sender->pointM, firstStep);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, 
                                 const Point2d& orignPt, bool firstStep)
{
    return sender->view->getSnap()->snapPoint(sender, orignPt,
        firstStep ? NULL : m_shape, m_step);
}

void MgCommandDraw::setStepPoint(int step, const Point2d& pt)
{
    if (step > 0) {
        dynshape()->shape()->setHandlePoint(step, pt, 0);
    }
}

bool MgCommandDraw::touchBeganStep(const MgMotion* sender)
{
    if (0 == m_step) {
        m_step = 1;
        Point2d pnt(snapPoint(sender, true));
        for (int i = dynshape()->shape()->getPointCount() - 1; i >= 0; i--) {
            dynshape()->shape()->setPoint(i, pnt);
        }
        setStepPoint(0, pnt);
    }
    else {
        setStepPoint(m_step, snapPoint(sender));
    }
    dynshape()->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCommandDraw::touchMovedStep(const MgMotion* sender)
{
    if (sender->dragging()) {
        setStepPoint(m_step, snapPoint(sender));
        dynshape()->shape()->update();
    }
    return MgCommandDraw::touchMoved(sender);
}

bool MgCommandDraw::touchEndedStep(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender));
    Tol tol(sender->displayMmToModel(2.f));
    
    setStepPoint(m_step, pnt);
    dynshape()->shape()->update();
    
    if (!pnt.isEqualTo(dynshape()->shape()->getPoint(m_step - 1), tol)) {
        m_step++;
        if (m_step >= getMaxStep()) {
            if (!dynshape()->shape()->getExtent().isEmpty(tol, false)) {
                addShape(sender);
            }
            m_step = 0;
        }
    }

    return MgCommandDraw::touchEnded(sender);
}
