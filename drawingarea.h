#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT
public:
    DrawingArea();

    static bool initFb(const char *dev);
    static void closeFb();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *data) override;

signals:

public slots:

private:
    void drawAAPixel(uchar *address, double distance, bool aa);
    void drawAALine(const QLine &line, bool aa);
    void drawThinLine(QLine line, int color);

    void sendUpdate(const QLine &area, int waveform);

    bool m_invert;
};

#endif // DRAWINGAREA_H
