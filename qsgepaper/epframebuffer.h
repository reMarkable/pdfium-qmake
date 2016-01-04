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

    //   Waveforms:
    // 0: ~780ms, initialize to white
    // 1: ~170ms, mono, no flashing
    // 2: ~460ms, 15 grayscale, flashing all pixels white -> black pixels
    // 3: ~460ms, 15, grayscale, flashing all pixels white -> black pixels
    // 4: ~460ms, 4 grayscale, flashing all pixels white -> black pixels
    // 5: ~460ms, 4 grayscale, flashing all pixels white -> black pixels
    // 6: ~135ms, no flashing
    // 7: ~300ms, white, 2 gray, 1 black, no flashing, stuck black?
    // 8: ~435ms, fast flashing, all gray, highlight, several grays, lowest color - 1 gets stuck
    // 9: ~2365ms, lots of flashing, delta something?

    enum Waveform {
        Initialize = 0,
        Mono = 1,
        Grayscale = 2,
        Highlight = 8
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
