// mgdrawlines.cpp
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawline.h"
#include "mgbasicsps.h"
#include "mgstorage.h"

bool MgCmdDrawLine::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgLine::Type(), sender, s);
}

bool MgCmdDrawRayLine::initialize(const MgMotion* sender, MgStorage* s)
{
    bool ret = _initialize(MgLine::Type(), sender, s);
    ((MgLine*)dynshape()->shape())->setRayline(true);
    return ret;
}

bool MgCmdDrawBeeLine::initialize(const MgMotion* sender, MgStorage* s)
{
    bool ret = _initialize(MgLine::Type(), sender, s);
    ((MgLine*)dynshape()->shape())->setBeeline(true);
    return ret;
}

bool MgCmdDrawLine::backStep(const MgMotion* sender)
{
    return MgCommandDraw::backStep(sender);
}

bool MgCmdDrawLine::touchBegan(const MgMotion* sender)
{
    m_step = 1;

    Point2d pnt(snapPoint(sender, true));
    dynshape()->shape()->setPoint(0, pnt);
    dynshape()->shape()->setPoint(1, pnt);
    dynshape()->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawLine::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(1, snapPoint(sender));
    dynshape()->shape()->update();

    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawLine::touchEnded(const MgMotion* sender)
{
    if ( ((MgLine*)dynshape()->shape())->length() > sender->displayMmToModel(2.f)) {
        addShape(sender);
    } else {
        sender->view->showMessage("@shape_too_small");
    }
    m_step = 0;

    return MgCommandDraw::touchEnded(sender);
}

// MgCmdDrawDot
//

bool MgCmdDrawDot::initialize(const MgMotion* sender, MgStorage* s)
{
    bool ret = _initialize(MgDot::Type(), sender, s);
    
    if (s) {
        ((MgDot*)dynshape()->shape())->setPointType(s->readInt("pttype", 0));
    }
    return ret;
}

bool MgCmdDrawDot::click(const MgMotion* sender)
{
    return touchBegan(sender) && touchEnded(sender);
}

bool MgCmdDrawDot::touchBegan(const MgMotion* sender)
{
    m_step = 1;
    dynshape()->shape()->setPoint(0, snapPoint(sender, true));
    dynshape()->shape()->update();
    
    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawDot::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(0, snapPoint(sender));
    dynshape()->shape()->update();
    
    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawDot::touchEnded(const MgMotion* sender)
{
    addShape(sender);
    m_step = 0;
    
    return MgCommandDraw::touchEnded(sender);
}
