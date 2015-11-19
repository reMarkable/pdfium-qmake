#include "epimagenode.h"

#include "../eptexture.h"

EPImageNode::EPImageNode() :
    QSGImageNode(),
    EPNode()
{

}

void EPImageNode::draw(QPainter *painter) const
{
    painter->save();
    painter->setTransform(transform);
    painter->drawImage(rect, m_sourceImage.scaled(rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter->restore();
}



void EPImageNode::setTargetRect(const QRectF &targetRect)
{
    rect = targetRect.toRect();
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
    EPTexture *texture = static_cast<EPTexture*>(t);
    m_sourceImage = texture->image;
    if (rect.isNull()) {
        rect = m_sourceImage.rect();
    }
    dirty = true;
}

void EPImageNode::setMirror(bool mirror)
{
    //TODO FIXME
    //image = image.mirrored();
    dirty = true;
}

void EPImageNode::setMipmapFiltering(QSGTexture::Filtering filtering)
{
}

void EPImageNode::setFiltering(QSGTexture::Filtering filtering)
{
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
