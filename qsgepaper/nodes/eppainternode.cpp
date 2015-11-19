#include "eppainternode.h"

EPPainterNode::EPPainterNode(QQuickPaintedItem *item) :
    QSGPainterNode(),
    EPNode(),
    m_item(item)
{
    rect = QRect(0, 0, item->width(), item->height());
    dirty = true;
}



void EPPainterNode::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);
    m_item->paint(painter);
    painter->restore();
}

bool EPPainterNode::fast()
{
    return false;
}

void EPPainterNode::setPreferredRenderTarget(QQuickPaintedItem::RenderTarget target)
{
    Q_UNUSED(target)
}

void EPPainterNode::setSize(const QSize &size)
{
    rect = QRect(QPoint(0, 0), size);
}

void EPPainterNode::setDirty(const QRect &dirtyRect)
{
    Q_UNUSED(dirtyRect)
}

void EPPainterNode::setOpaquePainting(bool opaque)
{
    Q_UNUSED(opaque)
}

void EPPainterNode::setLinearFiltering(bool linearFiltering)
{
    Q_UNUSED(linearFiltering)
}

void EPPainterNode::setMipmapping(bool mipmapping)
{
    Q_UNUSED(mipmapping)
}

void EPPainterNode::setSmoothPainting(bool s)
{
    Q_UNUSED(s)
}

void EPPainterNode::setFillColor(const QColor &c)
{
    Q_UNUSED(c)
}

void EPPainterNode::setContentsScale(qreal s)
{
    Q_UNUSED(s)
}

void EPPainterNode::setFastFBOResizing(bool dynamic)
{
    Q_UNUSED(dynamic)
}

QImage EPPainterNode::toImage() const
{
    return QImage();
}

void EPPainterNode::update()
{
}

QSGTexture *EPPainterNode::texture() const
{
    return nullptr;
}
