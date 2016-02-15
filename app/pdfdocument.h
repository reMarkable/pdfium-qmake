#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include "document.h"

#include <fpdfview.h>
#include <QMutex>

class PdfDocument : public Document
{
    Q_OBJECT

public:
    PdfDocument(QString path, QObject *parent = nullptr);
    virtual ~PdfDocument();

protected slots:
    virtual QImage loadOriginalPage(int index, QSize dimensions) override;

private:
    FPDF_DOCUMENT m_pdfDocument;
    QMutex m_destructionLock;
    bool m_initialized;
};

#endif // PDFDOCUMENT_H
