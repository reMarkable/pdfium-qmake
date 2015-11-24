#ifndef EPPAINTERNODE_H
#define EPPAINTERNODE_H

#include <QtQuick/private/qsgadaptationlayer_p.h>

#include "epnode.h"

class QQuickPaintedItem;

class EPPainterNode : public QSGPainterNode, public EPNode
{
public:
    EPPainterNode(QQuickPaintedItem *item);

    // QSGPainterNode interface
public:
    void setPreferredRenderTarget(QQuickPaintedItem::RenderTarget target) override;
    void setSize(const QSize &size) override;
    void setDirty(const QRect &dirtyRect) override;
    void setOpaquePainting(bool opaque) override;
    void setLinearFiltering(bool linearFiltering) override;
    void setMipmapping(bool mipmapping) override;
    void setSmoothPainting(bool s) override;
    void setFillColor(const QColor &c) override;
    void setContentsScale(qreal s) override;
    void setFastFBOResizing(bool dynamic) override;
    QImage toImage() const override;
    void update() override;
    QSGTexture *texture() const override;

    struct EPPainterNodeContent : public Content {
        void draw(QPainter *) const override;
        QQuickPaintedItem *m_item;
    };
};

#endif // EPPAINTERNODE_H
