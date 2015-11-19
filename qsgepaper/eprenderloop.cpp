#include "eprenderloop.h"
#include "epcontext.h"

#include <QtQuick/private/qquickwindow_p.h>

EPRenderLoop::EPRenderLoop() :
    QSGRenderLoop(),
    m_context(new EPContext),
    m_updating(false)
{
    m_animationDriver = m_context->createAnimationDriver(this);
}



void EPRenderLoop::show(QQuickWindow *window)
{
    addWindow(window);
    maybeUpdate(window);
}

void EPRenderLoop::hide(QQuickWindow *window)
{
    removeWindow(window);
}

void EPRenderLoop::windowDestroyed(QQuickWindow *window)
{
    removeWindow(window);
}

void EPRenderLoop::exposureChanged(QQuickWindow *window)
{
    maybeUpdate(window);
}

QImage EPRenderLoop::grab(QQuickWindow *window)
{
    qWarning() << Q_FUNC_INFO << "not implemented";
    return QImage();
}

void EPRenderLoop::update(QQuickWindow *window)
{
    maybeUpdate(window);
}

void EPRenderLoop::maybeUpdate(QQuickWindow *window)
{
    if (!windows().contains(window)) {
        qDebug() << "not contains";
        return;
    }

    QQuickWindowPrivate *d = QQuickWindowPrivate::get(window);
    if (!d->isRenderable()) {
       // qDebug() << "not renderable";
        return;
    }

    if (m_updating) {
     //   qDebug() << "already updating";
        return;
    }
    m_updating = true;

    d->flushDelayedTouchEvent();
    d->polishItems();
    d->syncSceneGraph();
    d->renderSceneGraph(window->size());
    d->fireFrameSwapped();
    //qDebug() << "animation driver running?" << m_animationDriver->isRunning();
    m_animationDriver->advance();

    m_updating = false;
}

QAnimationDriver *EPRenderLoop::animationDriver() const
{
//    qDebug(Q_FUNC_INFO);
    return m_animationDriver;
}

QSGContext *EPRenderLoop::sceneGraphContext() const
{
//    qDebug(Q_FUNC_INFO);
    return m_context;
}

QSGRenderContext *EPRenderLoop::createRenderContext(QSGContext *context) const
{
//    qDebug(Q_FUNC_INFO);
    return context->createRenderContext();
}

void EPRenderLoop::releaseResources(QQuickWindow *window)
{
    qDebug(Q_FUNC_INFO);
}

void EPRenderLoop::handleUpdateRequest(QQuickWindow *w)
{
    maybeUpdate(w);
}
