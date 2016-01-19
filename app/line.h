#ifndef LINE_H
#define LINE_H

#include <QObject>
#include <QVector>
#include "digitizer.h"

struct Line {
    Q_GADGET

public:
    enum Color {
        Black,
        Gray,
        White
    };
    Q_ENUM(Color)

    enum Brush {
        Paintbrush,
        Pencil,
        Pen,
        Eraser,
        ZoomTool,
        InvalidBrush = -1

    };
    Q_ENUM(Brush)

    Brush brush = InvalidBrush;
    Color color = Black;
    QVector<PenPoint> points;
};


#endif // LINE_H
