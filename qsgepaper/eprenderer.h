#ifndef EPRENDERER_H
#define EPRENDERER_H

#include <QtQuick/private/qsgrenderer_p.h>
#include <QtQuick/private/qsgadaptationlayer_p.h>
#include <QTimer>
#include <nodes/epnode.h>

class EPRenderContext;
class EPFrameBuffer;

class EPRenderer : public QSGRenderer, QSGNodeVisitorEx
{
    Q_OBJECT

public:
    EPRenderer(EPRenderContext *context);

public:
    // QSGNodeVisitorEx interface
    bool visit(QSGTransformNode *) override;
    void endVisit(QSGTransformNode *) override;
    bool visit(QSGImageNode *) override;
    void endVisit(QSGImageNode *) override;
    bool visit(QSGRectangleNode *) override;
    void endVisit(QSGRectangleNode *) override;
    bool visit(QSGGlyphNode *) override;
    void endVisit(QSGGlyphNode *) override;

    bool visit(QSGNinePatchNode *) override;
    void endVisit(QSGNinePatchNode *) override;
    bool visit(QSGPainterNode *node) override;
    void endVisit(QSGPainterNode *) override;
    bool visit(QSGRootNode *) override;
    void endVisit(QSGRootNode *) override;
    bool visit(QSGClipNode *) override;
    void endVisit(QSGClipNode *) override;
    bool visit(QSGGeometryNode *) override;
    void endVisit(QSGGeometryNode *) override;
    bool visit(QSGOpacityNode *) override;
    void endVisit(QSGOpacityNode *) override;

    QMutex rectanglesMutex;
    QList<EPNode*> currentRects;

signals:
    void renderComplete();

private slots:
    void drawRects();

protected:
    // QSGRenderer interface
    void render() override;

private:
    void handleEpaperNode(EPNode *node);

    EPRenderContext *m_context;
    QMatrix4x4 m_transform;
    int m_depth;
    QTimer m_redrawTimer;
};

#endif // EPRENDERER_H
