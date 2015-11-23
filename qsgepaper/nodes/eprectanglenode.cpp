#include "eprectanglenode.h"

#include <QPainter>

EPRectangleNode::EPRectangleNode() :
    QSGRectangleNode(),
    EPNode(),
    m_bgColor(Qt::transparent),
    m_fgColor(Qt::transparent),
    m_borderWidth(0),
    m_radius(0)
{
    //    qDebug(Q_FUNC_INFO);
}

void EPRectangleNode::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);

    painter->setBrush(m_bgColor);

    // Account for stupid QPainter drawing outside of the rectangle...
    QRect drawnRect(rect.x() + m_borderWidth,
                    rect.y() + m_borderWidth,
                    rect.width() - m_borderWidth * 2,
                    rect.height() - m_borderWidth * 2);

    painter->setPen(QPen(m_fgColor, m_borderWidth));
    if (m_radius) {
        painter->drawRoundedRect(drawnRect, m_radius, m_radius);
    } else {
        painter->drawRect(drawnRect);
    }
    painter->restore();
}

void EPRectangleNode::setRect(const QRectF &rectangle)
{
    if (rectangle == rect) {
        return;
    }

    rect = rectangle.toRect();
    dirty = true;
}

void EPRectangleNode::setColor(const QColor &color)
{
    if (color == m_bgColor) {
        return;
    }

    m_bgColor = color;
    dirty = true;
}

void EPRectangleNode::setPenColor(const QColor &color)
{
    if (color == m_fgColor) {
        return;
    }

    m_fgColor = color;
    dirty = true;
}

void EPRectangleNode::setPenWidth(qreal width)
{
    if (width == m_borderWidth) {
        return;
    }

    m_borderWidth = width;
    dirty = true;
}

void EPRectangleNode::setGradientStops(const QGradientStops &stops)
{
  //  qWarning() << "gradients not supported";
}

void EPRectangleNode::setRadius(qreal radius)
{
    if (radius == m_radius) {
        return;
    }

    m_radius = radius;
    dirty = true;
}

void EPRectangleNode::setAligned(bool aligned)
{
    //qWarning() << "alignment not supported" << aligned;
}

void EPRectangleNode::update()
{
}
