#ifndef PDFRENDERER_H
#define PDFRENDERER_H

#include <fpdfview.h>
#include <QImage>

class Document;

class PdfRenderer : public QObject
{
    Q_OBJECT

public:
    PdfRenderer(Document *document);
    ~PdfRenderer();

    QImage renderPage(int index, QSize dimensions);

    bool initialize();

signals:
    void pageCountChanged(int pageCount);

private:
    FPDF_DOCUMENT m_pdfDocument;
    QString m_path;
    int m_pageCount;
};

#endif//PDFRENDERER_H
