#include "eprenderer.h"

#include "epcontext.h"

#include "nodes/epglyphnode.h"
#include "nodes/eprectanglenode.h"
#include "nodes/epimagenode.h"
#include "nodes/eppainternode.h"
#include <algorithm>

#include "epframebuffer.h"

EPRenderer::EPRenderer(EPRenderContext *context) :
    QSGRenderer(context),
    m_context(context)
{
    m_fb = new EPFrameBuffer(this);
    m_redrawTimer.setInterval(10);
    m_redrawTimer.setSingleShot(true);
    connect(&m_redrawTimer, SIGNAL(timeout()), SIGNAL(renderComplete()));
}


void EPRenderer::render()
{
    //qDebug() << Q_FUNC_INFO;
    rectanglesMutex.lock();
    //qDebug() << Q_FUNC_INFO << "past mutex";

    for(EPNode *rect : currentRects) {
        rect->visible = false;
    }

    m_transform.setToIdentity();;
    m_depth = 0;
    visitChildren(rootNode());

    std::sort(currentRects.begin(), currentRects.end());

    rectanglesMutex.unlock();

    //emit renderComplete();
    m_redrawTimer.start();
}

void EPRenderer::handleEpaperNode(EPNode *node)
{
    node->transform = m_transform.toTransform();
    node->transformedRect = node->transform.mapRect(node->rect);
    node->z = m_depth;

    int index = currentRects.indexOf(node);
    if (index != -1) {
        currentRects[index]->visible = true;
        return;
    }
    node->dirty = true;
    node->visible = true;

    currentRects.append(node);
}


bool EPRenderer::visit(QSGTransformNode *node)
{
    m_depth++;
    m_transform *= node->matrix();
    return true;
}

void EPRenderer::endVisit(QSGTransformNode *node)
{
    m_depth--;
    m_transform *= node->matrix().inverted();
}

bool EPRenderer::visit(QSGImageNode *node)
{
    EPImageNode *imageNode = static_cast<EPImageNode*>(node);
    handleEpaperNode(imageNode);
    return true;
}

void EPRenderer::endVisit(QSGImageNode *)
{
}

bool EPRenderer::visit(QSGRectangleNode *node)
{
    EPRectangleNode *rectangleNode = static_cast<EPRectangleNode*>(node);
    handleEpaperNode(rectangleNode);
    return true;
}

void EPRenderer::endVisit(QSGRectangleNode *)
{
}

bool EPRenderer::visit(QSGGlyphNode *node)
{
    EPGlyphNode *glyphNode = static_cast<EPGlyphNode*>(node);
    handleEpaperNode(glyphNode);
    return true;
}

void EPRenderer::endVisit(QSGGlyphNode *)
{
}


bool EPRenderer::visit(QSGPainterNode *node)
{
    EPPainterNode *painterNode = static_cast<EPPainterNode*>(node);
    handleEpaperNode(painterNode);
    return true;
}

void EPRenderer::endVisit(QSGPainterNode *)
{
}

bool EPRenderer::visit(QSGClipNode *) { qDebug(Q_FUNC_INFO); return true; }

void EPRenderer::endVisit(QSGClipNode *) { qDebug(Q_FUNC_INFO); }

bool EPRenderer::visit(QSGGeometryNode *) { /*qDebug(Q_FUNC_INFO);*/ return true; }

void EPRenderer::endVisit(QSGGeometryNode *) { /*qDebug(Q_FUNC_INFO);*/ }

bool EPRenderer::visit(QSGOpacityNode *) { /*qDebug(Q_FUNC_INFO);*/ return true; }

void EPRenderer::endVisit(QSGOpacityNode *) { /*qDebug(Q_FUNC_INFO);*/ }

bool EPRenderer::visit(QSGNinePatchNode *) { qDebug(Q_FUNC_INFO); return true; }

void EPRenderer::endVisit(QSGNinePatchNode *) { qDebug(Q_FUNC_INFO); }

bool EPRenderer::visit(QSGRootNode *) { qDebug(Q_FUNC_INFO); return true; }

void EPRenderer::endVisit(QSGRootNode *) { qDebug(Q_FUNC_INFO); }
