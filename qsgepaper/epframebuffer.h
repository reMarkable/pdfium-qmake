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

private:
    EPFrameBuffer();
    QImage m_fb;
    QFile m_deviceFile;
    bool m_invert;
};

#endif // EPFRAMEBUFFER_H
