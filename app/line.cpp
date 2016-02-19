#include "line.h"

#include <QDataStream>
#include <QDebug>

QDataStream &operator<<(QDataStream &datastream, const PenPoint &point)
{
    datastream << point.x << point.y << point.pressure;
    return datastream;
}

QDataStream &operator>>(QDataStream &datastream, PenPoint &point)
{
    return datastream >> point.x >> point.y >> point.pressure;
}

QDataStream &operator<<(QDataStream &datastream, const Line &line)
{
    return datastream << line.brush << line.color << line.points;
    return datastream << line.brush << line.color << line.points;
}

QDataStream &operator>>(QDataStream &datastream, Line &line)
{
    int color;
    int brush;
    datastream >> brush >> color >> line.points;
    line.color = Line::Color(color);
    line.brush = Line::Brush(brush);
    return datastream;
}
