#include "pdfdocument.h"

#include <QDebug>
#include <QElapsedTimer>

#define BORDER 100

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

QImage PdfDocument::loadOriginalPage(int index, QSize dimensions)
{
    // TODO: FPDF_QuickDrawPage
    if (index < 0) {
        return QImage();
    }

    if (!m_pdfDocument) {
        QElapsedTimer documentTimer;
        documentTimer.start();
        m_pdfDocument = FPDF_LoadDocument(path().toLocal8Bit().constData(), nullptr);
        setPageCount(FPDF_GetPageCount(m_pdfDocument));
        qDebug() << "Document loaded in" << documentTimer.elapsed() << "ms";
    }

    if (index > pageCount()) {
        return QImage();
    }

    QElapsedTimer timer;
    timer.start();

    FPDF_PAGE pdfPage = FPDF_LoadPage(m_pdfDocument, index);


    QImage image(dimensions.width(), dimensions.height(), QImage::Format_Grayscale8);
    image.fill(Qt::white);
    FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(image.width(), image.height(),
                                             FPDFBitmap_Gray,
                                             image.scanLine(0),
                                             image.bytesPerLine());
    FPDF_RenderPageBitmap(bitmap, pdfPage, 0, 0, image.width(), image.height(), 0, 0);
    FPDFBitmap_Destroy(bitmap);
    FPDF_ClosePage(pdfPage);

#if CLIP_PDF
    int left = image.width() - 1, right = 0,
        top = image.height() - 1, bottom = 0;

    for (int y = 0; y < image.height(); ++y) {
        const uchar *row = image.constScanLine(y);
        bool rowFilled = false;
        for (int x = 0; x < image.width(); ++x) {
            if (row[x] == 0x0) {
                rowFilled = true;
                right = std::max(right, x);
                if (left > x) {
                    left = x;
                    x = right; // shortcut to only search for new right bound from here
                }
            }
        }
        if (rowFilled) {
            top = std::min(top, y);
            bottom = y;
        }
    }
    left = qMax(0, left - BORDER);
    right = qMin(image.width(), right + BORDER);
    top = qMax(0, top - BORDER);
    bottom = qMin(image.height(), bottom + BORDER);

    int width = right - left;
    int height = bottom - top;
    if (width < 1) {
        width = 1;
    }
    if (height < 1) {
        height = 1;
    }

    image = image.copy(left, top, width, height);
#endif

    if (image.width() > image.height()) {
        image = image.scaledToWidth(dimensions.width(), Qt::SmoothTransformation);
    } else {
        image = image.scaledToHeight(dimensions.height(), Qt::SmoothTransformation);
    }

    qDebug() << "Page" << index << " loaded in" << timer.elapsed() << "ms";

    return image.convertToFormat(QImage::Format_RGB16);
}
