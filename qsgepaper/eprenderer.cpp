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

    for (int i=0; i<currentRects.size(); i++) {
        currentRects[i]->visible = false;
    }

    m_transform.setToIdentity();;
    m_depth = 0;
    m_order = 0;
    visitChildren(rootNode());

    std::sort(currentRects.begin(), currentRects.end(), [](const std::shared_ptr<EPNode::Content> &a, const std::shared_ptr<EPNode::Content> &b){
        if (a->z == b->z) {
            return a->order < b->order;
        } else {
            return a->z < b->z;
        }
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

    QMutableListIterator<std::shared_ptr<EPNode::Content>> it(currentRects);
    while (it.hasNext()) {
        std::shared_ptr<EPNode::Content> rect = it.next();
        Q_ASSERT(rect.get());
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

    QRect totalDamaged;
    for(std::shared_ptr<EPNode::Content> rect : currentRects) {
        Q_ASSERT(rect.get());
        if (rect->dirty) {
            totalDamaged = totalDamaged.united(rect->transformedRect);
            damagedAreas.append(rect->transformedRect);
        }
    }

    // Rects are sorted in z-order
    for(std::shared_ptr<EPNode::Content> rect : currentRects) {
        if (rect->dirty) {
            rect->draw(&painter);
            rect->dirty = false;
            totalDamaged = totalDamaged.united(rect->transformedRect);
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

    qDebug() << "Damaged:" << totalDamaged << ((double)(100.0 * totalDamaged.height() * totalDamaged.width()) / (double)(fb->width() * fb->height())) << "%";

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
    if (((double)(100.0 * totalDamaged.height() * totalDamaged.width()) / (double)(fb->width() * fb->height())) > 90) {
        EPFrameBuffer::instance()->sendUpdate(fb->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::FullUpdate, true);
    } else {
        EPFrameBuffer::instance()->sendUpdate(fb->rect(), EPFrameBuffer::Grayscale, EPFrameBuffer::PartialUpdate, true);
    }
#endif

//    qDebug() << Q_FUNC_INFO << timer.restart() << "updated";
}

void EPRenderer::handleEpaperNode(EPNode *node)
{
    bool found = false;
    for (int i=0; i<currentRects.size(); i++) {
        if (currentRects[i].get() == node->content.get()) {
            found = true;
            break;
        }
    }

    if (!found) {
        node->content->dirty = true;
        currentRects.append(node->content);
    }

    node->content->transform = m_transform.toTransform();
    node->content->transformedRect = node->content->transform.mapRect(node->content->rect);
    node->content->z = m_depth;
    node->content->order = m_order++;
    node->content->visible = true;
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
