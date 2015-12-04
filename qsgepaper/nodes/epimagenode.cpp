#include "epimagenode.h"
#include <QElapsedTimer>

#include "../eptexture.h"

EPImageNode::EPImageNode() :
    QSGImageNode(),
    EPNode()
{
    content = std::make_shared<EPImageNodeContent>();
}

void EPImageNode::EPImageNodeContent::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);
    painter->drawImage(rect.topLeft(), m_scaledImage);
    painter->restore();
}

void EPImageNode::EPImageNodeContent::updateCached()
{
    if (!rect.isValid() || m_sourceImage.isNull()) {
        return;
    }
    QElapsedTimer timer;
    timer.start();
    m_scaledImage = m_sourceImage.scaled(rect.size(), Qt::IgnoreAspectRatio, m_transformationMode);
    qDebug() << Q_FUNC_INFO << "scaled image in" << timer.elapsed() << "ms";
}



void EPImageNode::setTargetRect(const QRectF &targetRect)
{
    EPImageNodeContent *p = static_cast<EPImageNodeContent*>(content.get());
    if (targetRect.toRect() == p->rect) {
        return;
    }
    p->rect = targetRect.toRect();
    p->updateCached();
}

void EPImageNode::setInnerTargetRect(const QRectF &rect)
{
}

void EPImageNode::setInnerSourceRect(const QRectF &rect)
{
}

void EPImageNode::setSubSourceRect(const QRectF &rect)
{
}

void EPImageNode::setTexture(QSGTexture *t)
{
    EPImageNodeContent *p = static_cast<EPImageNodeContent*>(content.get());
    EPTexture *texture = static_cast<EPTexture*>(t);
    p->m_sourceImage = texture->image;
    if (p->rect.isNull()) {
        p->rect = p->m_sourceImage.rect();
    }
    p->dirty = true;
    p->updateCached();
}

void EPImageNode::setMirror(bool mirror)
{
    //TODO FIXME
    //image = image.mirrored();
    EPImageNodeContent *p = static_cast<EPImageNodeContent*>(content.get());
    p->dirty = true;
}

void EPImageNode::setMipmapFiltering(QSGTexture::Filtering filtering)
{
}

void EPImageNode::setFiltering(QSGTexture::Filtering filtering)
{
    EPImageNodeContent *p = static_cast<EPImageNodeContent*>(content.get());
    if (filtering == QSGTexture::Linear) {
        p->m_transformationMode = Qt::SmoothTransformation;
    } else {
        p->m_transformationMode = Qt::FastTransformation;
    }
}

void EPImageNode::setHorizontalWrapMode(QSGTexture::WrapMode wrapMode)
{
}

void EPImageNode::setVerticalWrapMode(QSGTexture::WrapMode wrapMode)
{
}

void EPImageNode::update()
{
}
