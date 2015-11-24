#ifndef EPRECTANGLENODE_H
#define EPRECTANGLENODE_H

#include <QtQuick/private/qsgadaptationlayer_p.h>

#include "epnode.h"

class EPRectangleNode : public QSGRectangleNode, public EPNode
{
public:
    EPRectangleNode();

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

    struct EPRectangleNodeContent : public Content {
        void draw(QPainter *painter) const override;
        QColor m_bgColor = Qt::transparent;
        QColor m_fgColor = Qt::transparent;
        qreal m_borderWidth = 0;
        qreal m_radius = 0;
    };
};

#endif // RECTANGLENODE_H
