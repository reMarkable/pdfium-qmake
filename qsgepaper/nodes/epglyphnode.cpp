#include "epglyphnode.h"

EPGlyphNode::EPGlyphNode() :
    QSGGlyphNode(),
    EPNode(),
    m_color(Qt::white)
{
    setGeometry(new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 0));
}

void EPGlyphNode::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);
    painter->setPen(m_color);
    painter->drawGlyphRun(m_baseline, m_glyphs);
    painter->restore();
}

void EPGlyphNode::setGlyphs(const QPointF &position, const QGlyphRun &glyphs)
{
    QRectF r = glyphs.boundingRect().translated(position);
    r.moveTop(r.y() -  glyphs.boundingRect().height());
    rect = r.toRect();
    m_glyphs = glyphs;
    m_baseline = position;
    m_baseline.setY(m_baseline.y() + r.height() / 4);
    markDirty(QSGNode::DirtyForceUpdate);
    dirty = true;
}

void EPGlyphNode::setColor(const QColor &color)
{
    m_color = color;
    dirty = true;
}

void EPGlyphNode::setStyle(QQuickText::TextStyle style)
{
}

void EPGlyphNode::setStyleColor(const QColor &color)
{
}

QPointF EPGlyphNode::baseLine() const
{
    return m_baseline;
}

void EPGlyphNode::setPreferredAntialiasingMode(QSGGlyphNode::AntialiasingMode)
{
}

void EPGlyphNode::update()
{
    dirty = true;
}
