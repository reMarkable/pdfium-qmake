#ifndef EPIMAGENODE_H
#define EPIMAGENODE_H

#include <QtQuick/private/qsgadaptationlayer_p.h>

#include "epnode.h"
#include <QDebug>

class EPImageNode : public QSGImageNode, public EPNode
{
public:
    EPImageNode();

    // QSGImageNode interface
public:
    void setTargetRect(const QRectF &rect) override;
    void setInnerTargetRect(const QRectF &rect) override;
    void setInnerSourceRect(const QRectF &rect) override;
    void setSubSourceRect(const QRectF &rect) override;
    void setTexture(QSGTexture *texture) override;
    void setMirror(bool mirror) override;
    void setMipmapFiltering(QSGTexture::Filtering filtering) override;
    void setFiltering(QSGTexture::Filtering filtering) override;
    void setHorizontalWrapMode(QSGTexture::WrapMode wrapMode) override;
    void setVerticalWrapMode(QSGTexture::WrapMode wrapMode) override;
    void update() override;

    struct EPImageNodeContent : public Content {
        void draw(QPainter *painter) const override;
        void updateCached();

        QImage m_scaledImage;
        QImage m_sourceImage;
#ifdef TWO_PASS
        QImage m_monoImage;
#endif
        Qt::TransformationMode m_transformationMode = Qt::FastTransformation;
    };
};

#endif // EPIMAGENODE_H
