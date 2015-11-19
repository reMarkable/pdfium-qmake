#ifndef EPCONTEXT_H
#define EPCONTEXT_H

#include <QtQuick/private/qsgcontext_p.h>

class EPContext;
class QAnimationDriver;

class EPRenderContext : public QSGRenderContext
{
    Q_OBJECT

    // QSGRenderContext interface
public:
    EPRenderContext(EPContext *context);

    bool isValid() const override;
    void initialize(QOpenGLContext *context) override;
    void invalidate() override;
    void renderNextFrame(QSGRenderer *renderer, GLuint fboId) override;
    void endSync() override;
    QSharedPointer<QSGDepthStencilBuffer> depthStencilBufferForFbo(QOpenGLFramebufferObject *fbo) override;
    QSGDistanceFieldGlyphCache *distanceFieldGlyphCache(const QRawFont &font) override;
    QSGTexture *createTexture(const QImage &image) const override;
    QSGTexture *createTextureNoAtlas(const QImage &image) const override;
    QSGRenderer *createRenderer() override;
    void compile(QSGMaterialShader *shader, QSGMaterial *material, const char *vertexCode, const char *fragmentCode) override;
    void initialize(QSGMaterialShader *shader) override;

    EPContext *m_context;
};


class EPContext : public QSGContext
{
    Q_OBJECT

public:
    EPContext();

signals:

public slots:

    // QSGContext interface
public:
    void renderContextInitialized(QSGRenderContext *renderContext) override;
    void renderContextInvalidated(QSGRenderContext *renderContext) override;
    QSGRenderContext *createRenderContext() override;
    QSGRectangleNode *createRectangleNode() override;
    QSGImageNode *createImageNode() override;
    QSGGlyphNode *createGlyphNode(QSGRenderContext *rc, bool preferNativeGlyphNode) override;
    QSGPainterNode *createPainterNode(QQuickPaintedItem *item) override;
    QAnimationDriver *createAnimationDriver(QObject *parent) override;
    QSize minimumFBOSize() const override;
    QSurfaceFormat defaultSurfaceFormat() const override;

    QAnimationDriver *animationDriver;
};

#endif // EPCONTEXT_H
