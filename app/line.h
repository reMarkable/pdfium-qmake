#ifndef LINE_H
#define LINE_H

#include <QObject>
#include <QVector>
#include "penpoint.h"

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

QDataStream &operator<<(QDataStream &datastream, const PenPoint &point);

QDataStream &operator>>(QDataStream &datastream, PenPoint &point);

QDataStream &operator<<(QDataStream &datastream, const Line &line);

QDataStream &operator>>(QDataStream &datastream, Line &line);

#endif // LINE_H
