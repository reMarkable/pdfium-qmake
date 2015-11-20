#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QQuickPaintedItem>
#include <QImage>
#include "digitizer.h"

struct Predictor
{
    Predictor(double firstValue) :
        lastValue(firstValue),
        valueCount(0)
    {
    }

    double getPrediction(double newValue) {
        valueCount++;
        double delta = newValue - lastValue;

        if (valueCount == 2) {
            trendDelta = delta - lastDelta;
        } else if (valueCount > 2) {
            delta = smoothFactor * delta + (1.0 - smoothFactor) * (lastDelta + trendDelta);
            trendDelta  = trendFactor * (delta - lastDelta) + (1.0 - trendFactor) * trendDelta;
            double slowstartScale = pow(1.0 - 1.0 / valueCount, 3);
            newValue += delta + trendDelta * predictionFactor * slowstartScale;
        }

        lastValue = newValue;
        lastDelta = delta;

        return lastValue;
    }

    double lastValue;
    double lastDelta;
    double trendDelta;

    double predictionFactor = 7.5;
    double smoothFactor = 0.098;
    double trendFactor = 0.054;

    int valueCount;
};

class DrawingArea : public QQuickPaintedItem
{
    Q_OBJECT
    Q_ENUMS(Brush)
    Q_PROPERTY(Brush currentBrush READ currentBrush WRITE setCurrentBrush NOTIFY currentBrushChanged)
public:
    DrawingArea();

    enum Brush {
        Paintbrush,
        Pencil,
        Pen
    };
    Brush currentBrush() const { return m_currentBrush; }
    void setCurrentBrush(Brush brush) { m_currentBrush = brush; emit currentBrushChanged(); }

    void paint(QPainter *painter) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void currentBrushChanged();

private:
    bool m_invert;
    QVector<QVector<PenPoint>> m_lines;
    Brush m_currentBrush;
    QImage m_contents;
};

#endif // DRAWINGAREA_H
