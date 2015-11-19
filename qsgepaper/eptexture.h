#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickTextureFactory>

class EPTexture : public QSGTexture
{
public:
    EPTexture(const QImage &image);

    // QSGTexture interface
public:
    int textureId() const;
    QSize textureSize() const;
    bool hasAlphaChannel() const;
    bool hasMipmaps() const;
    QRectF normalizedTextureSubRect() const;
    bool isAtlasTexture() const;
    QSGTexture *removedFromAtlas() const;
    void bind();

    const QImage &image;
private:
    int m_id;
};

class EPTextureFactory : public QQuickTextureFactory
{
    Q_OBJECT

public:
    EPTextureFactory(const QImage &image);

    QSGTexture *createTexture(QQuickWindow * window) const override;
    QImage image() const override;
    int	textureByteCount() const override;
    QSize textureSize() const override;

private:
    QImage m_image;
};


#endif // TEXTURE_H
