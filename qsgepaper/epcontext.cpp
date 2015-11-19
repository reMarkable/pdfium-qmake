#include "epcontext.h"

#include "eptexture.h"
#include "nodes/eprectanglenode.h"
#include "nodes/epimagenode.h"
#include "nodes/epglyphnode.h"
#include "nodes/eppainternode.h"
#include "eprenderer.h"

EPContext::EPContext()
{
}



void EPContext::renderContextInitialized(QSGRenderContext *renderContext)
{
    Q_UNUSED(renderContext);
    qDebug(Q_FUNC_INFO);
}

void EPContext::renderContextInvalidated(QSGRenderContext *renderContext)
{
    Q_UNUSED(renderContext);
    qDebug(Q_FUNC_INFO);
}

QSGRenderContext *EPContext::createRenderContext()
{
//    qDebug(Q_FUNC_INFO);
    return new EPRenderContext(this);
}

QSGRectangleNode *EPContext::createRectangleNode()
{
    return new EPRectangleNode;
}

QSGImageNode *EPContext::createImageNode()
{
    return new EPImageNode;
}

QSGGlyphNode *EPContext::createGlyphNode(QSGRenderContext *rc, bool preferNativeGlyphNode)
{
    Q_UNUSED(rc)
    Q_UNUSED(preferNativeGlyphNode)

    return new EPGlyphNode;
}

QSGPainterNode *EPContext::createPainterNode(QQuickPaintedItem *item)
{
    return new EPPainterNode(item);
}

QAnimationDriver *EPContext::createAnimationDriver(QObject *parent)
{
    //qDebug(Q_FUNC_INFO);
    //TODO: custom animation driver to neuter animations
    animationDriver = new QAnimationDriver(parent);
    return animationDriver;
}

QSize EPContext::minimumFBOSize() const
{
    qDebug(Q_FUNC_INFO);
    return QSize(1, 1);
}

QSurfaceFormat EPContext::defaultSurfaceFormat() const
{
//    qDebug(Q_FUNC_INFO);
    return QSurfaceFormat();
}


EPRenderContext::EPRenderContext(EPContext *context) :
    QSGRenderContext(context),
    m_context(context)
{
//    qDebug(Q_FUNC_INFO);
}

bool EPRenderContext::isValid() const
{
    return true;
}

void EPRenderContext::initialize(QOpenGLContext *context)
{
    qDebug(Q_FUNC_INFO);
    Q_UNUSED(context)
}

void EPRenderContext::invalidate()
{
    qDebug(Q_FUNC_INFO);
}

void EPRenderContext::renderNextFrame(QSGRenderer *renderer, GLuint fboId)
{
    if (!renderer) {
        qWarning() << Q_FUNC_INFO << "can't render without a renderer";
        return;
    }

    renderer->renderScene(fboId);
    //m_context->animationDriver->advance();
}

void EPRenderContext::endSync()
{
}

QSharedPointer<QSGDepthStencilBuffer> EPRenderContext::depthStencilBufferForFbo(QOpenGLFramebufferObject *fbo)
{
    qDebug(Q_FUNC_INFO);
    Q_UNUSED(fbo)
    return QSharedPointer<QSGDepthStencilBuffer>();
}

QSGDistanceFieldGlyphCache *EPRenderContext::distanceFieldGlyphCache(const QRawFont &font)
{
    qDebug(Q_FUNC_INFO);
    Q_UNUSED(font)
    return nullptr;
}

QSGTexture *EPRenderContext::createTexture(const QImage &image) const
{
    qDebug(Q_FUNC_INFO);
    return new EPTexture(image);
}

QSGTexture *EPRenderContext::createTextureNoAtlas(const QImage &image) const
{
    qDebug(Q_FUNC_INFO);
    return new EPTexture(image);
}

QSGRenderer *EPRenderContext::createRenderer()
{
//    qDebug(Q_FUNC_INFO);
    return new EPRenderer(this);
}

void EPRenderContext::compile(QSGMaterialShader *shader, QSGMaterial *material, const char *vertexCode, const char *fragmentCode)
{
    Q_UNUSED(shader)
    Q_UNUSED(material)
    Q_UNUSED(vertexCode)
    Q_UNUSED(fragmentCode)
    qWarning() << Q_FUNC_INFO << "Shaders are not supported";
}

void EPRenderContext::initialize(QSGMaterialShader *shader)
{
    Q_UNUSED(shader)
    qWarning() << Q_FUNC_INFO << "Shaders are not supported";
}
