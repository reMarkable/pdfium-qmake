#ifndef GLYPHNODE_H
#define GLYPHNODE_H

#include <QtQuick/private/qsgadaptationlayer_p.h>

#include "epnode.h"

class EPGlyphNode : public QSGGlyphNode, public EPNode
{
public:
    EPGlyphNode();

    void draw(QPainter *painter) const override;

    bool fast() { return false; }

signals:

public slots:

    // QSGGlyphNode interface
public:
    void setGlyphs(const QPointF &position, const QGlyphRun &glyphs) override;
    void setColor(const QColor &color) override;
    void setStyle(QQuickText::TextStyle style) override;
    void setStyleColor(const QColor &color) override;
    QPointF baseLine() const override;
    void setPreferredAntialiasingMode(AntialiasingMode) override;
    void update() override;

private:
    QPointF m_baseline;
    QColor m_color;
    QGlyphRun m_glyphs;
};

#endif // GLYPHNODE_H
