#include "pdfrenderer.h"

#include "document.h"
#include <QDebug>

#define BORDER 100

static bool s_pdfiumInitialized = false;

PdfRenderer::PdfRenderer(Document *document) :
    m_pdfDocument(nullptr),
    m_path(document->path()),
    m_pageCount(0)
{
    connect(this, SIGNAL(pageCountChanged(int)), document, SLOT(setPageCount(int)));
}

PdfRenderer::~PdfRenderer()
{
    if (m_pdfDocument) {
        FPDF_CloseDocument(m_pdfDocument);
        m_pdfDocument = nullptr;
    }
}

QImage PdfRenderer::renderPage(int index, QSize dimensions)
{
    if (!m_pdfDocument) {
        qWarning() << "Not initialize!";
        return QImage();
    }

    if (index < 0) {
        return QImage();
    }

    if (index > m_pageCount) {
        return QImage();
    }


    // TODO: FPDF_QuickDrawPage?
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

    return image.convertToFormat(QImage::Format_RGB16);
}

bool PdfRenderer::initialize()
{
    if (m_pdfDocument) {
        qWarning() << "Trying to initialize twice!";
        return false;
    }

    if (!s_pdfiumInitialized) {
        FPDF_LIBRARY_CONFIG_ config;
        config.version = 2;
        config.m_pUserFontPaths = nullptr;
        config.m_pIsolate = nullptr;
        config.m_v8EmbedderSlot = 0;
#ifdef Q_PROCESSOR_ARM
        static const char *s_fontpaths[] = { "/system/fonts/", nullptr };
        config.m_pUserFontPaths = s_fontpaths;
#endif

        FPDF_InitLibraryWithConfig(&config);

        s_pdfiumInitialized = true;
    }

    m_pdfDocument = FPDF_LoadDocument(m_path.toLocal8Bit().constData(), nullptr);
    if (!m_pdfDocument) {
        qWarning() << "Failed to load PDF document";
        return false;
    }


    m_pageCount = FPDF_GetPageCount(m_pdfDocument);
    emit pageCountChanged(m_pageCount);

    return true;
}
