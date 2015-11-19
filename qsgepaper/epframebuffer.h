#ifndef EPFRAMEBUFFER_H
#define EPFRAMEBUFFER_H

#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtCore/QFile>

class EPRenderer;

class EPFrameBuffer : public QObject
{
    Q_OBJECT
public:
    explicit EPFrameBuffer(EPRenderer *renderer);

    enum Waveform {
        Initialize = 0,
        Fast = 6,
        Grayscale = 2
    };
    enum UpdateMode {
        PartialUpdate,
        FullUpdate
    };

public slots:
    void draw();
    void clearScreen();

private:
    void sendUpdate(QRect rect, Waveform waveform, UpdateMode mode, bool sync = false);
    EPRenderer *m_renderer;
    QImage m_fb;
    QFile m_deviceFile;
};

#endif // EPFRAMEBUFFER_H
