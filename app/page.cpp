#include "page.h"
#include <QDebug>

Page::Page(FPDF_PAGE page, QObject *parent) : QObject(parent),
    m_pdfPage(page)
{
}

Page::Page(QString backgroundPath, QObject *parent) : QObject(parent),
    m_backgroundPath(backgroundPath),
    m_pdfPage(nullptr)
{
//    for (int i=0; i<10; i++) {
//        Page::Line line;

//        for (int j=0; j<10; j++) {
//            PenPoint point;
//            point.pressure = 0.5;
//            point.x = double(qrand()) / RAND_MAX;
//            point.y = double(qrand()) / RAND_MAX;
//            line.points.append(point);
//        }

//        line.brush = Page::Pen;
//        line.color = Page::Black;
//        m_lines.append(line);
//    }
}

Page::Page(const Page &rhs) : QObject(rhs.parent())
{
    m_background = rhs.m_background;
    m_lines = rhs.m_lines;
}

Page &Page::operator=(const Page &rhs)
{
    m_background = rhs.m_background;
    m_lines = rhs.m_lines;
    return *this;
}

Page::~Page()
{
}

const QVector<Page::Line> &Page::lines() const
{
    return m_lines;
}

void Page::setLines(const QVector<Page::Line> &newLines)
{
    m_lines = newLines;
}

void Page::addLine(const Page::Line &line)
{
    m_lines.append(line);
}

const QImage &Page::background() const
{
    return m_background;
}

bool Page::hasBackground() const
{
    return !m_backgroundPath.isEmpty();
}

void Page::loadBackground()
{
    if (!m_backgroundPath.isEmpty()) {
        m_background = QImage(m_backgroundPath).convertToFormat(QImage::Format_RGB16);
        
        if (m_background.height() > 1540) {
            m_background = m_background.scaledToHeight(1540, Qt::SmoothTransformation);
        }
    } else if (m_pdfPage) {
        m_background = QImage(1200, 1540, QImage::Format_Grayscale8);
        m_background.fill(Qt::white);
        FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(m_background.width(), m_background.height(),
                                                 FPDFBitmap_Gray,
                                                 m_background.scanLine(0),
                                                 m_background.bytesPerLine());
        FPDF_RenderPageBitmap(bitmap, m_pdfPage, 0, 0, m_background.width(), m_background.height(), 0, 0);
        FPDFBitmap_Destroy(bitmap);
        m_background = m_background.convertToFormat(QImage::Format_RGB16);
    } else {
        qWarning() << "Trying to load but nothing to load";
        return;
    }

    emit backgroundLoaded();
}
