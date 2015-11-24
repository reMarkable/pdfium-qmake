#include "epimagenode.h"

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
    painter->drawImage(rect, m_scaledImage);
    painter->restore();
}



void EPImageNode::setTargetRect(const QRectF &targetRect)
{
    EPImageNodeContent *p = static_cast<EPImageNodeContent*>(content.get());
    p->rect = targetRect.toRect();
    p->m_scaledImage = p->m_sourceImage.scaled(p->rect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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
