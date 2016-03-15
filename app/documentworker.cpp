#include "documentworker.h"

#include "collection.h"
#include "pdfrenderer.h"
#include "settings.h"
#include "document.h"
#include <QFile>
#include <QMap>
#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QTimer>

#define CACHE_COUNT 2 // Cache this many before and after

namespace { // Local to this file
struct TemplateLoader {
    TemplateLoader() {
        QDir dir(Collection::collectionPath() + "/templates/");
        QFileInfoList files = dir.entryInfoList(QStringList("*.png"));
        for (QFileInfo fileInfo : files) {
            templates.insert(fileInfo.baseName(), QImage(fileInfo.absoluteFilePath()).convertToFormat(QImage::Format_RGB16));
        }
    }

    QMap<QString, QImage> templates;
};
}

static TemplateLoader s_templateLoader;

DocumentWorker::DocumentWorker(Document *document) :
    QThread(document),
    pageDirty(false),
    m_document(document),
    m_suspended(false),
    m_pdfRenderer(nullptr)
{
    connect(document, SIGNAL(currentPageChanged(int)), this, SLOT(onPageChanged(int)), Qt::DirectConnection);
    connect(document, SIGNAL(missingThumbnailRequested(int)), this, SLOT(onMissingThumbnailRequested(int)));
    connect(document, SIGNAL(pagesDeleted(QList<int>)), this, SLOT(deletePages(QList<int>)));
    connect(document, SIGNAL(templateChanged()), this, SLOT(onTemplateChanged()), Qt::DirectConnection);
    connect(this, SIGNAL(backgroundsLoaded(int,QImage)), this, SLOT(onBackgroundLoaded(int,QImage)));
    connect(this, SIGNAL(pageLoaded(int,QImage)), this, SLOT(onPageLoaded(int,QImage)));
    connect(this, SIGNAL(thumbnailUpdated(int)), document, SIGNAL(thumbnailUpdated(int)));

    m_currentPage = m_document->currentPage();
    m_currentTemplate = m_document->currentTemplate();


    if (document->path().endsWith(".pdf")) {
        m_pdfRenderer = new PdfRenderer(document);
    } else {
        if (s_templateLoader.templates.isEmpty()) {
            qWarning() << "No templates available!";
        }
    }

    QTimer::singleShot(0, this, [=]() {
        start(QThread::LowestPriority);
    });
}

DocumentWorker::~DocumentWorker()
{
    if (m_pdfRenderer) {
        m_pdfRenderer->deleteLater();
    }
}

void DocumentWorker::suspend()
{
    QMutexLocker locker(&m_lock);
    m_suspended = true;
}

void DocumentWorker::wake()
{
    QMutexLocker locker(&m_lock);
    m_suspended = false;
    m_suspendCondition.wakeAll();
}

QImage DocumentWorker::background()
{
    if (m_pdfRenderer) {
        qDebug() << "Cached backgrounds contain this page?:" << m_cachedBackgrounds.contains(m_currentPage);
        return m_cachedBackgrounds.value(m_currentPage);
    }

    qDebug() << "current template:" <<  m_currentTemplate << "valid?" << s_templateLoader.templates.keys().contains(m_document->currentTemplate());
    QImage background = s_templateLoader.templates.value(m_currentTemplate);

    QString thumbnailPath = m_document->getThumbnailPath(m_currentPage);
    if (!QFile::exists(thumbnailPath) && !hasLinesForPage(m_currentPage)) {
        background.scaled(Settings::thumbnailWidth(), Settings::thumbnailHeight()).save(thumbnailPath);
        emit thumbnailUpdated(m_currentPage);
    }

    return background;
}

void DocumentWorker::onPageChanged(int newPage)
{
    qDebug() << "Page changed" << newPage << m_currentPage;

    if (newPage == m_currentPage) {
        return;
    }

    QMutexLocker locker(&m_lock);

    // If we have drawn on the current page, we need to store it
    if (pageDirty) {
        qDebug() << "Dirty page, storing";
        m_cachedContents.insert(m_currentPage, pageContents);
        m_pagesToStore.insert(m_currentPage, pageContents);
        pageDirty = false;
        m_waitCondition.wakeAll();
    }
    pageContents = QImage();
    qDebug() << "Changing page";

    const int cacheMin = qMax(newPage - CACHE_COUNT, 0);
    const int cacheMax = newPage + CACHE_COUNT;

    foreach(int page, m_cachedBackgrounds.keys()) {
        if (page >= cacheMin && page <= cacheMax) {
            continue;
        }
        m_cachedBackgrounds.remove(page);
    }

    foreach(int page, m_cachedContents.keys()) {
        if (page >= cacheMin && page <= cacheMax) {
            continue;
        }
        m_cachedContents.remove(page);
    }

    // Ensure that we aren't currently trying to load anything
    clearLoadQueue();

    m_currentPage = newPage;
    m_currentTemplate = m_document->templateForPage(newPage);

    if (m_cachedContents.contains(newPage)) {
        pageContents = m_cachedContents.value(newPage);
        qDebug() << "Cache contains page";// << pageContents.isNull();
    } else if (!hasLinesForPage(newPage)) {
        if (m_pdfRenderer) {
            if (m_cachedBackgrounds.contains(m_currentPage)) {
                emit redrawingNeeded();
            }
        }
    }
    emit updateNeeded();

    locker.unlock();

    preload();
}

const QVector<Line> &DocumentWorker::lines()
{
    if (m_lines.count() <= m_document->currentPage()) {
        m_lines.resize(m_document->currentPage() + 1);
    }

    return m_lines[m_document->currentPage()];
}

void DocumentWorker::addLine(Line line)
{
    if (!m_document) {
        qWarning() << "Tried to add line, but no document available?";
    }
    if (m_lines.count() <= m_document->currentPage()) {
        m_lines.resize(m_document->currentPage() + 1);
    }
    m_lines[m_document->currentPage()].append(line);
}

Line DocumentWorker::popLine()
{
    return m_lines[m_currentPage].takeLast();
}

void DocumentWorker::storeThumbnail()
{
    QMutexLocker locker(&m_lock);
    qDebug() << "Storing thumbnail";

    QFile::remove(m_document->getThumbnailPath(m_currentPage));
    m_thumbnailsToCreate.insert(m_currentPage, pageContents);
    m_waitCondition.wakeAll();
}

void DocumentWorker::stop()
{
    qDebug() << "Someone stopped us";
    QMutexLocker locker(&m_lock);
    clearLoadQueue();
    m_waitCondition.wakeAll();
    m_suspendCondition.wakeAll();
    requestInterruption();
}

void DocumentWorker::run()
{
    loadLines();

    if (m_pdfRenderer) {
        if (!m_pdfRenderer->initialize()) {
            delete m_pdfRenderer;
            m_pdfRenderer = nullptr;
        }
    }

    while (!isInterruptionRequested()) {
        QMutexLocker locker(&m_lock);

        if (m_suspended) {
            qDebug() << "We're suspended";
            m_suspendCondition.wait(&m_lock);
            qDebug() << "Woken up again";
            continue;
        }

        if (m_pagesToLoad.isEmpty()
                && m_backgroundsToLoad.isEmpty()
                && m_pagesToStore.isEmpty()
                && m_thumbnailsToCreate.isEmpty()) {
            m_waitCondition.wait(&m_lock);
            continue;
        }

        if (!m_pagesToLoad.isEmpty()) { // Prioritize loading pages
            int page = m_pagesToLoad.keys().first();
            QString path = m_pagesToLoad.take(page);
            locker.unlock();

            qDebug() << "loading path:" << path;
            QImage image(path);
            if (QFile::exists(path) && image.isNull()) {
                QFile::remove(path); // Nuke invalid image
            }
            emit pageLoaded(page, image);
        } else if (!m_backgroundsToLoad.isEmpty()) {
            if (!m_pdfRenderer) {
                m_backgroundsToLoad.clear();
                continue;
            }

            int page = m_backgroundsToLoad.takeFirst();
            locker.unlock();

            qDebug() << "Rendering pdf page";
            QImage image = m_pdfRenderer->renderPage(page, QSize(1200, 1525));
            qDebug() << "PDF page is null?:" << image.isNull();
            emit backgroundsLoaded(page, image);
            QString thumbnailPath = m_document->getThumbnailPath(page);
            if (!QFile::exists(thumbnailPath)) {
                image.scaledToHeight(512).save(thumbnailPath);
                emit thumbnailUpdated(page);
            }
        } else if (!m_pagesToStore.isEmpty()) {
            qDebug() << "Storing page...";
            while (!m_pagesToStore.isEmpty()) {
                int page = m_pagesToStore.keys().first();
                QImage image = m_pagesToStore.take(page);
                QString path = m_document->getStoredPagePath(page);

                if (image.isNull() && QFile::exists(path)) {
                    qWarning() << "Refusing to store null page";
                    QFile::remove(path);
                    continue;
                }

                locker.unlock();

                image.save(path);
                locker.relock();
            }
        } else if (!m_thumbnailsToCreate.isEmpty()) {
            qDebug() << "Creating thumbnailo";
            int page = m_thumbnailsToCreate.keys().first();
            QImage contents = m_thumbnailsToCreate.take(page);

            QString thumbnailPath = m_document->getThumbnailPath(page);
            locker.unlock();

            if (contents.isNull()) {
                qDebug() << thumbnailPath << "existando?: " << QFile::exists(thumbnailPath);
                if (QFile::exists(thumbnailPath)) {
                    qDebug() << "Thumbnail for page" << page << "already exists, not overwriting";
                    continue;
                }

                if (m_pdfRenderer) {
                    contents = m_pdfRenderer->renderPage(page, QSize(Settings::thumbnailWidth(), Settings::thumbnailHeight()));
                } else {
                    qDebug() << "Storing thumbnail based on page contents";
                    QString contentPath = m_document->getStoredPagePath(page);
                    if (!QFile::exists(contentPath)) {
                        qWarning() << "Unable to load page content for page" << page;
                        continue;
                    }

                    contents = QImage(contentPath).scaled(Settings::thumbnailWidth(),
                                                          Settings::thumbnailHeight(),
                                                          Qt::KeepAspectRatioByExpanding,
                                                          Qt::SmoothTransformation);
                }
            } else {
                    contents = contents.scaled(Settings::thumbnailWidth(),
                                                          Settings::thumbnailHeight(),
                                                          Qt::KeepAspectRatioByExpanding,
                                                          Qt::SmoothTransformation);
            }

            if (contents.isNull()) {
                qWarning() << "Impossible situation, got null image";
            }
            contents.save(thumbnailPath);
            qDebug() << "Stored thumbnail to" << thumbnailPath;
            emit thumbnailUpdated(page);
        }
    }
    qDebug() << "Dying...";

    QMutexLocker locker(&m_lock);

    if (pageDirty) {
        // Store cached page content
        QString path = m_document->getStoredPagePath(m_currentPage);
        qDebug() << "Page dirty, storing in" << path;
        if (pageContents.isNull()) {
            QFile::remove(path);
        } else {
            pageContents.save(path);
        }
        qDebug() << pageContents.size();

    }

    // Store thumbnail
    if (!pageContents.isNull()) {
        QImage thumbnail = pageContents.scaled(Settings::thumbnailWidth(),
                                               Settings::thumbnailHeight(),
                                               Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);
        thumbnail.save(m_document->getThumbnailPath(m_currentPage));
        emit thumbnailUpdated(m_currentPage);
    }

    storeLines();

    deleteLater();
}

void DocumentWorker::onPageLoaded(int page, QImage contents)
{
    if (page == m_currentPage) {
        if (pageDirty || m_cachedContents.contains(page)) {
            qWarning() << "Loaded page, after page dirty";
            return;
        }
        qDebug() << "contents are null?:" << contents.isNull();
        pageContents = contents;
        qDebug() << "page loaded, requesting update";
        if (pageContents.isNull()) {
            emit redrawingNeeded();
        } else {
            emit updateNeeded();
        }
        return;
    }

    if (contents.isNull()) {
        m_cachedContents.remove(page);
    } else {
        m_cachedContents[page] = contents;
    }
}

void DocumentWorker::onBackgroundLoaded(int page, QImage contents)
{
    m_cachedBackgrounds[page] = contents;

    qDebug() << "Background loaded for page" << page << "current page:" << m_currentPage;
    if (page == m_currentPage && !QFile::exists(m_document->getStoredPagePath(page))) {
        qDebug() << "Current background loaded, cached document doesn't exist";
        if (pageContents.isNull()) {
            emit redrawingNeeded();
        } else {
            emit updateNeeded();
        }
    }
}

void DocumentWorker::preload()
{
    QMutexLocker locker(&m_lock);

    if (!m_cachedContents.contains(m_currentPage) && !pageDirty) {
        qDebug() << "We don't have the current page cached, loading";
        m_pagesToLoad.insert(m_currentPage, m_document->getStoredPagePath(m_currentPage));
    }

    if (m_pdfRenderer && !m_cachedBackgrounds.contains(m_currentPage) && !m_backgroundsToLoad.contains(m_currentPage)) {
        qDebug() << "Loading PDF background for page" << m_currentPage;
        m_backgroundsToLoad.append(m_currentPage);
    }

    for (int i=1; i<CACHE_COUNT; i++) {
        { // Cache forwards
            const int indexForward = m_currentPage + i;
            if (!m_cachedContents.contains(indexForward)) {
                m_pagesToLoad.insert(indexForward, m_document->getStoredPagePath(indexForward));
            }
            if (!m_cachedBackgrounds.contains(indexForward) && !m_backgroundsToLoad.contains(indexForward)) {
                m_backgroundsToLoad.append(indexForward);
            }
        }

        { // Cache backwards
            const int indexBackward = m_currentPage - i;
            if (!m_cachedContents.contains(indexBackward)) {
                m_pagesToLoad.insert(indexBackward, m_document->getStoredPagePath(indexBackward));
            }
            if (!m_cachedBackgrounds.contains(indexBackward) && !m_backgroundsToLoad.contains(indexBackward)) {
                m_backgroundsToLoad.append(indexBackward);
            }
        }
    }
    m_waitCondition.wakeAll();
}

void DocumentWorker::pruneCache()
{
    QMutexLocker locker(&m_lock);

    foreach(int index, m_cachedBackgrounds.keys()) {
        // Only keep the current page
        if (index != m_currentPage) {
            m_cachedBackgrounds.remove(index);
        }
    }

    foreach(int index, m_cachedContents.keys()) {
        // Only keep the current page
        if (index != m_currentPage) {
            m_cachedContents.remove(index);
        }
    }

    clearLoadQueue();
}

void DocumentWorker::printMemoryUsage() const
{
#if 0
    size_t linesMem = 0;
    linesMem += sizeof m_lines;
    for (const int key : m_lines.keys()) {
        linesMem += sizeof key;
        const QVector<Line> &lines = m_lines.value(key);
        linesMem += sizeof lines;
        for (const Line &line : lines) {
            linesMem += sizeof line;
            linesMem += line.points.count() * sizeof(PenPoint);
        }
    }
    qDebug() << "Memory used for lines:" << (linesMem / 1024.0) << "kb";

    size_t contentsMem = 0;
    for (const int key : m_pageContents.keys()) {
        const QImage &content = m_pageContents.value(key);
        contentsMem += sizeof content;
        contentsMem += content.byteCount();
    }
    qDebug() << "Memory used for contents:" << (contentsMem / 1024.0) << "kb";

    size_t backgroundsMem = 0;
    for (const int key : m_cachedBackgrounds.keys()) {
        const QImage &background = m_cachedBackgrounds.value(key);
        backgroundsMem += sizeof background;
        backgroundsMem += background.byteCount();
    }
    qDebug() << "Memory used for backgrounds:" << (backgroundsMem / 1024.0) << "kb";
    qDebug() << "Total memory used:" << ((backgroundsMem + contentsMem + linesMem + sizeof(*this)) / (1024.0 * 1024.0)) << "mb";
#endif
}

void DocumentWorker::clearLoadQueue()
{
    m_backgroundsToLoad.clear();
    m_pagesToLoad.clear();
}

bool DocumentWorker::hasLinesForPage(int page)
{
    if (page > 0 && page < m_lines.count()) {
        if (!m_lines.value(page).isEmpty()) {
            return true;
        }
    }

    return false;
}

void DocumentWorker::deletePages(QList<int> pagesToRemove)
{
    qDebug() << "Deleting pages:" << pagesToRemove;
    if (m_document->pageCount() < pagesToRemove.count()) {
        return;
    }

    QMutexLocker locker(&m_lock);
    // If we have drawn on the current page, we need to store it
    if (pageDirty) {
        m_pagesToStore.insert(m_currentPage, pageContents);
        m_waitCondition.wakeAll();
        pageDirty = false;
    }

    // So we avoid having to move everything around
    m_cachedBackgrounds.clear();
    m_cachedContents.clear();
    clearLoadQueue();

    QMap<int, int> oldPages;
    for (int i=0; i<m_document->pageCount(); i++) {
        oldPages[i] = i;
    }
    qDebug() << m_lines.count() << oldPages.count();

    QVector<QVector<Line>> newLines;
    int pagesTaken = 0;
    for (int oldPage : oldPages.keys()) {
        if (pagesToRemove.contains(oldPage)) {
            QFile::remove(m_document->getThumbnailPath(oldPage));
            QFile::remove(m_document->getStoredPagePath(oldPage));
            continue;
        }

        // Move thumbnail
        QString oldPath(m_document->getThumbnailPath(oldPages[oldPage]));
        QString newPath(m_document->getThumbnailPath(pagesTaken));
        QFile::rename(oldPath, newPath);

        // Move cached page content
        oldPath = m_document->getStoredPagePath(oldPages[oldPage]);
        newPath = m_document->getStoredPagePath(pagesTaken);
        QFile::rename(oldPath, newPath);

        newLines.append(m_lines.value(oldPage));

        pagesTaken++;
    }

    m_lines = newLines;
    qDebug() << "Removed pages";
}

void DocumentWorker::onMissingThumbnailRequested(int page)
{
    qDebug() << "Missing thumbnail requested";

    QMutexLocker locker(&m_lock);
    if (m_thumbnailsToCreate.contains(page)) {
        return;
    }

    QImage contents;
    if (!m_pdfRenderer) {
        QString pageTemplate = m_document->templateForPage(page);
        if (s_templateLoader.templates.contains(pageTemplate)) {
            contents = s_templateLoader.templates.value(pageTemplate);
        } else {
            qWarning() << "Invalid template:" << pageTemplate << "for page" << page;
        }
    }

    m_thumbnailsToCreate.insert(page, contents);
    m_waitCondition.wakeAll();
}

void DocumentWorker::onTemplateChanged()
{
    qDebug() << "Template changed";
    // PDF documents don't care about templates
    if (m_pdfRenderer) {
        return;
    }
    QString newTemplate = m_document->currentTemplate();
    if (newTemplate == m_currentTemplate) {
        qDebug() << "Not a new template";
        return;
    }
    qDebug() << newTemplate << m_currentTemplate;
    m_currentTemplate = newTemplate;

    // Invalidate this page
    QFile::remove(m_document->getThumbnailPath(m_currentPage));
    pageContents = QImage();
    m_cachedContents.remove(m_currentPage);

    emit redrawingNeeded();
}

void DocumentWorker::loadLines()
{
    QElapsedTimer timer;
    timer.start();
    QFile lineFile(m_document->path() + ".lines");
    if (!lineFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open" << lineFile.fileName();
        return;
    }

    QVector<QVector<Line>> pages;
    QVector<Line> pageLines;
    Line line;
    while (!lineFile.atEnd()) {
        QByteArray bytes = lineFile.readLine(256); // at most 256 bytes
        if (!bytes.endsWith('\n')) {
            qWarning() << "Missing newline in" << lineFile.fileName() << bytes;
            break;
        }

        // Remove the newline at the end
        bytes = bytes.trimmed();
        QList<QByteArray> parts = bytes.split(' ');

        if (bytes.startsWith("linestart ")) {
            if (parts.count() != 3) {
                qWarning() << "Invalid linestart in" << lineFile.fileName() << bytes;
                break;
            }
            bool ok;

            // Read brush
            line.brush = Line::Brush(parts[1].toInt(&ok));
            if (!ok) {
                qWarning() << "Invalid brush in" << lineFile.fileName() << bytes;
                break;
            }

            line.color = Line::Color(parts[2].toInt(&ok));
            if (!ok) {
                qWarning() << "Invalid color in" << lineFile.fileName() << bytes;
                break;
            }
        } else if (bytes == "lineend") {
            pageLines.append(line);
            line = Line();
        } else if (bytes == "pageend") {
            pages.append(pageLines);
            pageLines = QVector<Line>();
        } else {
            if (parts.count() != 3) {
                qWarning() << "Invalid line in" << lineFile.fileName() << bytes;
                break;
            }
            PenPoint point;
            bool ok;
            point.x = parts[0].toDouble(&ok);
            if (!ok) {
                qWarning() << "Invalid point.x in" << lineFile.fileName() << bytes;
                break;
            }

            point.y = parts[1].toDouble(&ok);
            if (!ok) {
                qWarning() << "Invalid point.y in" << lineFile.fileName() << bytes;
                break;
            }

            point.pressure = parts[2].toDouble(&ok);
            if (!ok) {
                qWarning() << "Invalid point.pressure in" << lineFile.fileName() << bytes;
                break;
            }
            point.pressure = qBound(0.f, point.pressure, 1.f);

            line.points.append(point);
        }
    }

    m_lock.lock();
    m_lines = pages;
    m_lock.unlock();

    qDebug() << "lines loaded in" << timer.elapsed() << "ms";
}

void DocumentWorker::storeLines()
{
    QString linePath = m_document->path() + ".lines";
    QFile lineFile(linePath + ".new");
    if (lineFile.open(QIODevice::WriteOnly)) {
        qDebug() << "storing lines in" << lineFile.fileName();
        QElapsedTimer timer;
        timer.start();
        for (const QVector<Line> &pageLines : m_lines) {

            // Skip all lines before a clear, to ensure that we don't store more lines than necessary to redraw.
            int linesStart = 0;
            for (int i=0; i<pageLines.count(); i++) {
                if (pageLines[i].brush == Line::InvalidBrush) {
                    linesStart = i + 1;
                }
            }

            for (int i = linesStart; i < pageLines.count(); i++) {
                const Line &line = pageLines[i];

                lineFile.write("linestart ");
                lineFile.write(QByteArray::number(line.brush) + ' ');
                lineFile.write(QByteArray::number(line.color) + ' ');
                lineFile.write("\n");

                for (const PenPoint &point : line.points) {
                    // At least 17 significant digits are enough to ensure that we don't lose precision
                    lineFile.write(QByteArray::number(point.x, 'g', 17) + ' ');
                    lineFile.write(QByteArray::number(point.y, 'g', 17) + ' ');
                    lineFile.write(QByteArray::number(point.pressure, 'g', 17));
                    lineFile.write("\n");
                }
                lineFile.write("lineend\n");
            }
            lineFile.write("pageend\n");
        }

        qDebug() << "lines stored in" << timer.elapsed();
        lineFile.close();

        // Ensure that we don't delete the existing file in case of a failure
        if (QFile::exists(linePath + ".old")) {
            QFile::remove(linePath + ".old");
        }
        if (!QFile::rename(linePath, linePath + ".old")) {
            qWarning() << "Unable to move old lines";
        }
        if (lineFile.rename(linePath)) {
            QFile::remove(linePath + ".old");
        } else {
            qWarning() << "Unable to rename new file";
        }
    } else {
        qWarning() << "Unable to open line file" << lineFile.fileName() << "for storing lines";
    }
}
