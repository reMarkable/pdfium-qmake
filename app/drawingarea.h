#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>
#include "digitizer.h"
#include "page.h"
#include <QPointer>

#ifdef Q_PROCESSOR_ARM
#include <epframebuffer.h>
#endif

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Page::Brush currentBrush MEMBER m_currentBrush NOTIFY currentBrushChanged)
    Q_PROPERTY(bool zoomtoolSelected MEMBER m_zoomSelected NOTIFY zoomtoolSelectedChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(Page::Color currentColor MEMBER m_currentColor NOTIFY currentColorChanged)
    Q_PROPERTY(Page* page MEMBER m_page WRITE setPage)

public:
    DrawingArea();

    void paint(QPainter *painter) override;

public slots:
    void clear();
    void undo();
    void redo();
    void setZoom(double x, double y, double width, double height);
    qreal zoomFactor() { return m_zoomFactor; }

    void setPage(Page *page);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

signals:
    void currentBrushChanged();
    void zoomtoolSelectedChanged();
    void zoomFactorChanged();
    void currentColorChanged();

private slots:
    void redrawBackbuffer();

private:
    void doZoom();
    void handleGesture();
    QRect lineBoundingRect(const QLine &line);
#ifdef Q_PROCESSOR_ARM
    void sendUpdate(QRect rect, const EPFrameBuffer::Waveform waveform);
#endif

    bool m_invert;
    Page::Brush m_currentBrush;
    QImage m_contents;
    bool m_hasEdited;
    QVector<Page::Line> m_lines;
    QList<Page::Line> m_undoneLines;
    double m_zoomFactor;
    QRectF m_zoomRect;
    bool m_zoomSelected;
    Page::Color m_currentColor;
    QPointer<Page> m_page;
};

#endif // DRAWINGAREA_H
