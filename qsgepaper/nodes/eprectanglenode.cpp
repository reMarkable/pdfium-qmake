#include "eprectanglenode.h"

#include <QPainter>

EPRectangleNode::EPRectangleNode() :
    QSGRectangleNode(),
    EPNode()
{
    content = std::make_shared<EPRectangleNodeContent>();
}

void EPRectangleNode::EPRectangleNodeContent::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);

#if TWO_PASS
    if (painter->renderHints() & QPainter::Antialiasing) {
        painter->setBrush(m_bgColor);
        painter->setPen(QPen(m_fgColor, m_borderWidth));
    } else {
        if (m_bgColor != Qt::black && m_bgColor != Qt::white) {
            painter->setBrush(Qt::transparent);
        } else {
            painter->setBrush(m_bgColor);
        }

        if (m_fgColor != Qt::black && m_fgColor != Qt::white) {
            painter->setPen(QPen(Qt::transparent, m_borderWidth));
        } else {
            painter->setPen(QPen(m_fgColor, m_borderWidth));
        }
    }
#else
    painter->setBrush(m_bgColor);
    painter->setPen(QPen(m_fgColor, m_borderWidth));
#endif

    // Account for stupid QPainter drawing outside of the rectangle...
    QRect drawnRect(rect.x() + m_borderWidth,
                    rect.y() + m_borderWidth,
                    rect.width() - m_borderWidth * 2,
                    rect.height() - m_borderWidth * 2);

    if (m_radius) {
        painter->drawRoundedRect(drawnRect, m_radius, m_radius);
    } else {
        painter->drawRect(drawnRect);
    }
    painter->restore();
}

void EPRectangleNode::setRect(const QRectF &rectangle)
{
    EPRectangleNodeContent *p = static_cast<EPRectangleNodeContent*>(content.get());
    if (rectangle == p->rect) {
        return;
    }

    p->rect = rectangle.toRect();
    p->dirty = true;
}

void EPRectangleNode::setColor(const QColor &color)
{
    EPRectangleNodeContent *p = static_cast<EPRectangleNodeContent*>(content.get());
    if (color == p->m_bgColor) {
        return;
    }

    p->m_bgColor = color;
    p->dirty = true;
}

void EPRectangleNode::setPenColor(const QColor &color)
{
    EPRectangleNodeContent *p = static_cast<EPRectangleNodeContent*>(content.get());
    if (color == p->m_fgColor) {
        return;
    }

    p->m_fgColor = color;
    p->dirty = true;
}

void EPRectangleNode::setPenWidth(qreal width)
{
    EPRectangleNodeContent *p = static_cast<EPRectangleNodeContent*>(content.get());
    if (width == p->m_borderWidth) {
        return;
    }

    p->m_borderWidth = width;
    p->dirty = true;
}

void EPRectangleNode::setGradientStops(const QGradientStops &stops)
{
  //  qWarning() << "gradients not supported";
}

void EPRectangleNode::setRadius(qreal radius)
{
    EPRectangleNodeContent *p = static_cast<EPRectangleNodeContent*>(content.get());
    if (radius == p->m_radius) {
        return;
    }

    p->m_radius = radius;
    p->dirty = true;
}

void EPRectangleNode::setAligned(bool aligned)
{
    //qWarning() << "alignment not supported" << aligned;
}

void EPRectangleNode::update()
{
}
