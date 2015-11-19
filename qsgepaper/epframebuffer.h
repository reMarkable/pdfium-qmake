#ifndef EPFRAMEBUFFER_H
#define EPFRAMEBUFFER_H

#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtCore/QFile>

class EPFrameBuffer : public QObject
{
    Q_OBJECT
public:
    static EPFrameBuffer *instance();

    enum Waveform {
        Initialize = 0,
        Fast = 6,
        Grayscale = 2
    };
    enum UpdateMode {
        PartialUpdate = 0x0,
        FullUpdate = 0x1
    };

    QImage *framebuffer() {
        return &m_fb;
    }

public slots:
    void clearScreen();
    void sendUpdate(QRect rect, Waveform waveform, UpdateMode mode, bool sync = false);
    void drawAAPixel(uchar *address, double distance, bool aa);
    void drawAALine(const QLine &line, bool aa);
    void drawThinLine(QLine line, int color);
    void drawThickLine(QLine line, int color, float pressure);
    void drawSinglePoint(int x, int y, int color);
    void drawFullPoint(int cx, int cy, int color, float size);
    uchar *getAddress(int x, int y);

private:
    EPFrameBuffer();
    QImage m_fb;
    QFile m_deviceFile;
    bool m_invert;
};

#endif // EPFRAMEBUFFER_H
