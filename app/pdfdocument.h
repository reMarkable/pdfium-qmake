#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include "document.h"

#include <fpdfview.h>

class PdfDocument : public Document
{
public:
    PdfDocument(QString path, QObject *parent = nullptr);
    virtual ~PdfDocument();

protected slots:
    virtual QImage loadOriginalPage(int index, QSize dimensions) override;

private:
    FPDF_DOCUMENT m_pdfDocument;
};

#endif // PDFDOCUMENT_H
