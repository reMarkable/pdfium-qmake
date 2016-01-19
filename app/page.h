#ifndef PAGE_H
#define PAGE_H

#include <QObject>
#include <QImage>
#include "digitizer.h"
#include <fpdfview.h>

class Page : public QObject
{
    Q_OBJECT
public:

    Page(FPDF_PAGE page, QObject *parent = 0);
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
    FPDF_PAGE m_pdfPage;
};

#endif // PAGE_H
