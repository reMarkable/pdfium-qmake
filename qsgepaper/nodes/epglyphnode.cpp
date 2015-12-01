#include "epglyphnode.h"

EPGlyphNode::EPGlyphNode() :
    QSGGlyphNode(),
    EPNode()
{
    content = std::make_shared<EPGlyphNodeContent>();
    setGeometry(new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 0));
}

void EPGlyphNode::EPGlyphNodeContent::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);

    //painter->setRenderHint(QPainter::Antialiasing);

    const QVector<quint32> indices = m_glyphs.glyphIndexes();
    const QVector<QPointF> positions = m_glyphs.positions();
    painter->translate(m_baseline);
    for (int i=0; i<indices.size(); i++) {
        QPainterPath path = m_glyphs.rawFont().pathForGlyph(indices[i]);
        painter->translate(positions[i].x(), 0);
        painter->fillPath(path, m_color);;
        painter->translate(-positions[i].x(), 0);
    }

    //painter->drawGlyphRun(m_baseline, m_glyphs);

    painter->restore();
}

void EPGlyphNode::setGlyphs(const QPointF &position, const QGlyphRun &glyphs)
{
    EPGlyphNodeContent *p = static_cast<EPGlyphNodeContent*>(content.get());
    QRectF r = glyphs.boundingRect().translated(position);
    r.moveTop(r.y() -  glyphs.boundingRect().height());
    p->rect = r.toRect();
    p->m_glyphs = glyphs;
    p->m_baseline = position;
    markDirty(QSGNode::DirtyForceUpdate);
    p->dirty = true;
}

void EPGlyphNode::setColor(const QColor &color)
{
    EPGlyphNodeContent *p = static_cast<EPGlyphNodeContent*>(content.get());
    p->m_color = color;
    p->dirty = true;
}

void EPGlyphNode::setStyle(QQuickText::TextStyle style)
{
}

void EPGlyphNode::setStyleColor(const QColor &color)
{
}

QPointF EPGlyphNode::baseLine() const
{
    EPGlyphNodeContent *p = static_cast<EPGlyphNodeContent*>(content.get());
    return p->m_baseline;
}

void EPGlyphNode::setPreferredAntialiasingMode(QSGGlyphNode::AntialiasingMode)
{
}

void EPGlyphNode::update()
{
    EPGlyphNodeContent *p = static_cast<EPGlyphNodeContent*>(content.get());
    p->dirty = true;
}
