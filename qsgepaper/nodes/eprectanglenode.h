#ifndef EPRECTANGLENODE_H
#define EPRECTANGLENODE_H

#include <QtQuick/private/qsgadaptationlayer_p.h>

#include "epnode.h"

class EPRectangleNode : public QSGRectangleNode, public EPNode
{
public:
    EPRectangleNode();

    void draw(QPainter *painter) const override;
    bool fast() { return true; }

signals:

public slots:

    // QSGRectangleNode interface
public:
    void setRect(const QRectF &rect) override;
    void setColor(const QColor &color) override;
    void setPenColor(const QColor &color) override;
    void setPenWidth(qreal width) override;
    void setGradientStops(const QGradientStops &stops) override;
    void setRadius(qreal radius) override;
    void setAligned(bool aligned) override;
    void update() override;

private:
    QColor m_bgColor;
    QColor m_fgColor;
    qreal m_borderWidth;
    qreal m_radius;
};

#endif // RECTANGLENODE_H
