#ifndef PDFWORKER_H
#define PDFWORKER_H

#include <fpdfview.h>
#include <QImage>

class Document;

class PDFWorker : public QObject
{
    Q_OBJECT

public:
    PDFWorker(Document *document);
    ~PDFWorker();

    QImage loadOriginalPage(int index, QSize dimensions);

    bool initialize();

signals:
    void pageCountChanged(int pageCount);

private:
    FPDF_DOCUMENT m_pdfDocument;
    QString m_path;
    int m_pageCount;
};

#endif // PDFWORKER_H
