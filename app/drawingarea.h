#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>
#include "line.h"
#include "document.h"
#include <QPointer>

#ifdef Q_PROCESSOR_ARM
#include <epframebuffer.h>
#endif

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Line::Brush currentBrush MEMBER m_currentBrush NOTIFY currentBrushChanged)
    Q_PROPERTY(bool zoomtoolSelected MEMBER m_zoomSelected NOTIFY zoomtoolSelectedChanged)
    Q_PROPERTY(qreal zoomFactor READ zoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(Line::Color currentColor MEMBER m_currentColor NOTIFY currentColorChanged)
    Q_PROPERTY(Document* document MEMBER m_document WRITE setDocument)

public:
    DrawingArea();
    ~DrawingArea();

    void paint(QPainter *painter) override;

public slots:
    void clear();
    void undo();
    void redo();
    void setZoom(double x, double y, double width, double height);
    qreal zoomFactor() { return m_zoomFactor; }

    void setDocument(Document *document);

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
    void drawBackground(QPainter *painter);
    void doZoom();
    void handleGesture();
    QRect lineBoundingRect(const QLine &line);
#ifdef Q_PROCESSOR_ARM
    void sendUpdate(QRect rect, const EPFrameBuffer::Waveform waveform);
#endif

    bool m_invert;
    Line::Brush m_currentBrush;
    QImage m_contents;
    bool m_hasEdited;
    QList<Line> m_undoneLines;
    double m_zoomFactor;
    QRectF m_zoomRect;
    bool m_zoomSelected;
    Line::Color m_currentColor;
    QPointer<Document> m_document;
};

#endif // DRAWINGAREA_H
