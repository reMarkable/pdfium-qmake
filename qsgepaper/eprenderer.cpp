#include "eprenderer.h"

#include "epcontext.h"

#include "nodes/epglyphnode.h"
#include "nodes/eprectanglenode.h"
#include "nodes/epimagenode.h"
#include "nodes/eppainternode.h"
#include <algorithm>
#include <QElapsedTimer>

#include "epframebuffer.h"

EPRenderer::EPRenderer(EPRenderContext *context) :
    QSGRenderer(context),
    m_context(context)
{
    m_redrawTimer.setInterval(10);
    m_redrawTimer.setSingleShot(true);
    connect(&m_redrawTimer, SIGNAL(timeout()), SLOT(drawRects()));
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

    std::sort(currentRects.begin(), currentRects.end(), [](EPNode *a, EPNode *b){
        return a->z < b->z;
    });

    rectanglesMutex.unlock();

    //emit renderComplete();
    m_redrawTimer.start();
}

void EPRenderer::drawRects()
{
    QImage *fb = EPFrameBuffer::instance()->framebuffer();
    if (fb->isNull()) {
        qWarning() << "Can't draw without a framebuffer";
    }

    QElapsedTimer timer;
    timer.start();
    QMutexLocker locker(&rectanglesMutex);

    QList<QRect> damagedAreas;

    QMutableListIterator<EPNode*> it(currentRects);
    while (it.hasNext()) {
        EPNode *rect = it.next();
        if (!rect->visible) {
            damagedAreas.append(rect->transformedRect);
            it.remove();
        } else if (!rect->transformedRect.intersects(fb->rect())) {
            it.remove();
        }
    }

    QPainter painter(fb);
    painter.setBackground(Qt::white);

    for(const QRect &area : damagedAreas) {
        painter.eraseRect(area);
    }

    for(EPNode *rect : currentRects) {
        if (rect->dirty) {
            damagedAreas.append(rect->transformedRect);
        }
    }

    // Rects are sorted in z-order
    for(EPNode *rect : currentRects) {
        if (rect->dirty) {
            rect->draw(&painter);
            //painter.drawRect(QRect(rect->transformedRect.x(), rect->transformedRect.y(), rect->transformedRect.width() - 1, rect->transformedRect.height()-1));
            //painter.drawText(rect->transformedRect.bottomLeft(), QString::number(rect->z));
            rect->dirty = false;
            continue;
        }

        for (const QRect &area : damagedAreas) {
            if (rect->transformedRect.intersects(area)) {
                rect->draw(&painter);
                damagedAreas.append(rect->transformedRect);
                break;
            }
        }
    }
    qDebug() << Q_FUNC_INFO << timer.restart() << "drawed";

    locker.unlock();
    painter.end();


#if 0
    // FIXME: do something more clever here plz
    for (int i=0; i<fastAreas.size(); i++) {
        if (fastAreas[i].isEmpty()) {
            continue;
        }
        for (int j=0; j<fastAreas.size(); j++) {
            if (i==j) continue;
            if (fastAreas[i].intersects(fastAreas[j])) {
                fastAreas[i] = fastAreas[i].united(fastAreas[j]);
                fastAreas[j] = QRect();
            }
        }
    }

    qDebug() << "count" << damagedAreas.count();
    if (damagedAreas.count() > 20) {
        sendUpdate(m_fb.rect(), Grayscale, PartialUpdate, true);
    } else {
        int i = 0;
        for(const QRect &area : fastAreas) {
            if (area.isEmpty()) {
                continue;
            }
            sendUpdate(area, Fast, PartialUpdate);
            i++;
        }
        qDebug() << "damaged areas:" << i;
    }

#else
    EPFrameBuffer::instance()->sendUpdate(fb->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::FullUpdate, true);
#endif

//    qDebug() << Q_FUNC_INFO << timer.restart() << "updated";
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
