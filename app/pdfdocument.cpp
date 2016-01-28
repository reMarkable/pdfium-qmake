#include "pdfdocument.h"

#include <QDebug>
#include <QElapsedTimer>

PdfDocument::PdfDocument(QString path, QObject *parent) :
    Document(path, parent),
    m_pdfDocument(nullptr)
{
}

PdfDocument::~PdfDocument()
{
    if (m_pdfDocument) {
        if (m_pdfDocument) {
            FPDF_CloseDocument(m_pdfDocument);
        }
    }
}

void PdfDocument::loadOriginalPage(int index)
{
    // TODO: FPDF_QuickDrawPage
    if (index < 0) {
        return;
    }

    if (!m_pdfDocument) {
        QElapsedTimer documentTimer;
        documentTimer.start();
        m_pdfDocument = FPDF_LoadDocument(path().toLocal8Bit().constData(), nullptr);
        setPageCount(FPDF_GetPageCount(m_pdfDocument));
        qDebug() << "Document loaded in" << documentTimer.elapsed() << "ms";
    }

    if (index > pageCount()) {
        return;
    }

    if (cachedIndices().contains(index)) {
        return;
    }

    QElapsedTimer timer;
    timer.start();

    FPDF_PAGE pdfPage = FPDF_LoadPage(m_pdfDocument, index);

    qDebug() << "Page" << index << " loaded in" << timer.elapsed() << "ms";

    QImage image(dimensions(), QImage::Format_Grayscale8);
    image.fill(Qt::white);
    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(),
                                             FPDFBitmap_Gray,
                                             image.scanLine(0),
                                             image.bytesPerLine());
    FPDF_RenderPageBitmap(bitmap, pdfPage, 0, 0, image.width(), image.height(), 0, 0);
    FPDFBitmap_Destroy(bitmap);
    FPDF_ClosePage(pdfPage);

    emit backgroundLoaded(image.convertToFormat(QImage::Format_RGB16), index);

    qDebug() << "Page" << index << " rendered in" << timer.elapsed() << "ms";
}
