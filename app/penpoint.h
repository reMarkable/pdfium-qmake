#ifndef PENPOINT_H
#define PENPOINT_H

struct PenPoint {
    PenPoint() : x(-1), y(-1), pressure(-1) {}
    PenPoint(double nx, double ny, float p) : x(nx), y(ny), pressure(p) {}
    double x;
    double y;
    float pressure;

    bool isValid() { return (x != -1 && y != -1 && pressure != -1); }
};

#endif // PENPOINT_H
