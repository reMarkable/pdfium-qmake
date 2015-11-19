#ifndef EPRENDERLOOP_H
#define EPRENDERLOOP_H

#include <QtQuick/private/qsgrenderloop_p.h>

class EPContext;

class EPRenderLoop : public QSGRenderLoop
{
public:
    EPRenderLoop();

signals:

public slots:

    // QSGRenderLoop interface
public:
    void show(QQuickWindow *window) override;
    void hide(QQuickWindow *window) override;
    void windowDestroyed(QQuickWindow *window) override;
    void exposureChanged(QQuickWindow *window) override;
    QImage grab(QQuickWindow *window) override;
    void update(QQuickWindow *window) override;
    void maybeUpdate(QQuickWindow *window) override;
    QAnimationDriver *animationDriver() const override;
    QSGContext *sceneGraphContext() const override;
    QSGRenderContext *createRenderContext(QSGContext *context) const override;
    void releaseResources(QQuickWindow *window) override;
    virtual void handleUpdateRequest(QQuickWindow *) override;

private:
    EPContext *m_context;
    QAnimationDriver *m_animationDriver;

    bool m_updating;

};

#endif // EPRENDERLOOP_H
