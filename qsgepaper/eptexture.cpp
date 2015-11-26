#include "eptexture.h"

EPTexture::EPTexture(const QImage &i) :
    image(i)
{
    static int id = 0;
    m_id = ++id;
}



int EPTexture::textureId() const
{
    return m_id;
}

QSize EPTexture::textureSize() const
{
    return image.size();
}

bool EPTexture::hasAlphaChannel() const
{
    return image.hasAlphaChannel();
}

bool EPTexture::hasMipmaps() const
{
    return false;
}

QRectF EPTexture::normalizedTextureSubRect() const
{
    return image.rect();
}

bool EPTexture::isAtlasTexture() const
{
    return false;
}

QSGTexture *EPTexture::removedFromAtlas() const
{
    return nullptr;
}

void EPTexture::bind()
{
}


//The QQuickTextureFactory class provides an interface for loading custom textures from QML.
//The purpose of the texture factory is to provide a placeholder for a image data that can be converted into an OpenGL texture.
//Creating a texture directly is not possible as there is rarely an OpenGL context available in the thread that is responsible for loading the image data.
EPTextureFactory::EPTextureFactory(const QImage &image)
{
    m_image = image.convertToFormat(QImage::Format_Indexed8);
}

//This function is called on the scene graph rendering thread to create a QSGTexture instance from the factory. window provides the context which this texture is created in.
//QML will internally cache the returned texture as needed. Each call to this function should return a unique instance.
//The OpenGL context used for rendering is bound when this function is called.
QSGTexture *EPTextureFactory::createTexture(QQuickWindow *window) const
{
    Q_UNUSED(window)

    return new EPTexture(m_image);
}

//Returns an image version of this texture.
//The lifespan of the returned image is unknown, so the implementation should return a self contained QImage, not make use of the QImage(uchar *, ...) constructor.
//This function is not commonly used and is expected to be slow.
QImage EPTextureFactory::image() const
{
    return m_image;
}

//Returns the number of bytes of memory the texture consumes.
int EPTextureFactory::textureByteCount() const
{
    return m_image.byteCount();
}

//Returns the size of the texture. This function will be called from arbitrary threads and should not rely on an OpenGL context bound.
QSize EPTextureFactory::textureSize() const
{
    return m_image.size();
}

