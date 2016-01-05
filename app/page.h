#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QImage>
#include "digitizer.h"

class Page : public QObject
{
    Q_OBJECT

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


public:
    struct Line {
        Brush brush = InvalidBrush;
        Color color = Black;
        QVector<PenPoint> points;
    };

    Page(QString backgroundPath, QObject *parent = 0);
    Page(const Page &);
    Page &operator=(const Page &);
    ~Page();

    const QVector<Line> &lines() const;
    void setLines(const QVector<Line> &newLines);
    void addLine(const Line &line);
    const QImage &background() const;
    bool hasBackground() const;

public slots:
    void loadBackground();
    QString backgroundPath() const { return m_backgroundPath; }

signals:
    void backgroundLoaded();

private:
    QVector<Page::Line> m_lines;
    QString m_backgroundPath;
    QImage m_background;
};

#endif // PAGE_H
