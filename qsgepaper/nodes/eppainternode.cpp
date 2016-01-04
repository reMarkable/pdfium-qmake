#include "eppainternode.h"

EPPainterNode::EPPainterNode(QQuickPaintedItem *item) :
    QSGPainterNode(),
    EPNode()
{
    content = std::make_shared<EPPainterNodeContent>();
    EPPainterNodeContent *p = static_cast<EPPainterNodeContent*>(content.get());
    p->m_item = item;
    p->rect = QRect(0, 0, item->width(), item->height());
    p->dirty = true;
}

void EPPainterNode::EPPainterNodeContent::draw(QPainter *painter) const
{
    Q_ASSERT(m_item);
    painter->save();
    painter->setTransform(transform);
    m_item->paint(painter);
    painter->restore();
}

void EPPainterNode::setPreferredRenderTarget(QQuickPaintedItem::RenderTarget target)
{
    Q_UNUSED(target)
}

void EPPainterNode::setSize(const QSize &size)
{
    EPPainterNodeContent *p = static_cast<EPPainterNodeContent*>(content.get());
    p->rect = QRect(QPoint(0, 0), size);
}

void EPPainterNode::setDirty(const QRect &dirtyRect)
{
    EPPainterNodeContent *p = static_cast<EPPainterNodeContent*>(content.get());
    p->dirty = true;
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
    EPPainterNodeContent *p = static_cast<EPPainterNodeContent*>(content.get());
    p->dirty = true;
}

QSGTexture *EPPainterNode::texture() const
{
    return nullptr;
}
