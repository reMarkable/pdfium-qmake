#include "documentworker.h"

#include "collection.h"
#include "pdfworker.h"
#include "settings.h"
#include "document.h"
#include <QFile>
#include <QMap>
#include <QDir>
#include <QDebug>

#define CACHE_COUNT 2 // Cache this many before and after

namespace { // Local to this file
struct TemplateLoader {
    TemplateLoader() {
        QDir dir(Collection::collectionPath() + "/templates/");
        QFileInfoList files = dir.entryInfoList(QStringList("*.png"));
        for (QFileInfo fileInfo : files) {
            templates.insert(fileInfo.baseName(), QImage(fileInfo.absoluteFilePath()));
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
    m_pdfWorker(nullptr)
{
    connect(document, SIGNAL(currentPageChanged(int)), this, SLOT(onPageChanged(int)));
    connect(document, SIGNAL(missingThumbnailRequested(int)), this, SLOT(onMissingThumbnailRequested(int)));
    connect(this, SIGNAL(backgroundsLoaded(int,QImage)), this, SLOT(onBackgroundLoaded(int,QImage)));
    connect(this, SIGNAL(pageLoaded(int,QImage)), this, SLOT(onPageLoaded(int,QImage)));
    connect(this, SIGNAL(thumbnailUpdated(int)), document, SIGNAL(thumbnailUpdated(int)));

    m_currentPage = m_document->currentPage();

    QFile lineFile(m_document->path() + ".lines");
    if (lineFile.open(QIODevice::ReadOnly)) {
        QDataStream dataStream(&lineFile);
        dataStream >> m_lines;
    }

    if (document->path().endsWith(".pdf")) {
        m_pdfWorker = new PDFWorker(document);
    } else {
        if (s_templateLoader.templates.isEmpty()) {
            qWarning() << "No templates available!";
            return;
        }
    }
}

DocumentWorker::~DocumentWorker()
{
    requestInterruption();
    m_waitCondition.wakeAll();

    wait();

    QMutexLocker locker(&m_lock);

    QFile lineFile(m_document->path() + ".lines");
    if (lineFile.open(QIODevice::WriteOnly)) {
        QDataStream dataStream(&lineFile);
        dataStream << m_lines;
    }

    if (m_pdfWorker) {
        m_pdfWorker->deleteLater();
    }


    qDebug() << "document worker nuked";
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
    if (m_pdfWorker) {
        return m_cachedBackgrounds.value(m_document->currentPage());
    }

    return s_templateLoader.templates.value(m_document->currentTemplate());
}

void DocumentWorker::onPageChanged(int newPage)
{
    if (newPage == m_currentPage) {
        return;
    }

    QMutexLocker locker(&m_lock);

    // If we have drawn on the current page, we need to store it
    if (pageDirty) {
        m_pagesToStore.insert(m_currentPage, pageContents);
        pageDirty = false;
        pageContents = QImage();
        m_waitCondition.wakeAll();
    }

    const int cacheMin = qMax(newPage - CACHE_COUNT, 0);
    const int cacheMax = newPage + CACHE_COUNT;

    foreach(int page, m_cachedBackgrounds.keys()) {
        if (page > cacheMin && page < cacheMax) {
            continue;
        }
        m_cachedBackgrounds.remove(page);
    }

    foreach(int page, m_cachedContents.keys()) {
        if (page > cacheMin && page < cacheMax) {
            continue;
        }
        m_cachedContents.remove(page);
    }

    clearLoadQueue();

    m_currentPage = newPage;

    locker.unlock();

    if (m_cachedContents.contains(newPage)) {
        emit currentPageLoaded();
    } else if (m_cachedBackgrounds.contains(newPage)) {
        emit currentPageLoaded();
    }

    preload();
}

const QVector<Line> &DocumentWorker::lines()
{
    if (m_lines.count() < m_document->currentPage()) {
        m_lines.resize(m_document->currentPage() + 1);
    }
    return m_lines[m_document->currentPage()];
}

void DocumentWorker::addLine(Line line)
{
    m_lines[m_document->currentPage()].append(line);
}

Line DocumentWorker::popLine()
{
    return m_lines[m_currentPage].takeLast();
}

void DocumentWorker::run()
{
    if (m_pdfWorker) {
        if (!m_pdfWorker->initialize()) {
            delete m_pdfWorker;
            m_pdfWorker = nullptr;
        }
    }

    while (!isInterruptionRequested()) {
        QMutexLocker locker(&m_lock);

        if (m_suspended) {
            m_suspendCondition.wait(&m_lock);
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
            emit pageLoaded(page, QImage(path));
        } else if (!m_backgroundsToLoad.isEmpty()) {
            if (!m_pdfWorker) {
                m_backgroundsToLoad.clear();
                continue;
            }

            int page = m_backgroundsToLoad.takeFirst();
            locker.unlock();

            QImage image = m_pdfWorker->loadOriginalPage(page, QSize(1200, 1600));
            emit backgroundsLoaded(page, image);
            QString thumbnailPath = m_document->getThumbnailPath(page);
            if (!QFile::exists(thumbnailPath)) {
                image.scaledToHeight(512).save(thumbnailPath);
                emit thumbnailUpdated(page);
            }
        } else if (!m_pagesToStore.isEmpty()) {
            int page = m_pagesToStore.keys().first();
            QImage image = m_pagesToStore.take(page);
            locker.unlock();

            image.save(m_document->getStoredPagePath(page));
            image.scaledToHeight(512).save(m_document->getThumbnailPath(page));
            emit thumbnailUpdated(page);
        } else if (!m_thumbnailsToCreate.isEmpty()) {
            if (!m_pdfWorker) {
                qDebug() << "Thumbnail requested, but no PDF worker";
                m_thumbnailsToCreate.clear();
                continue;
            }

            int page = m_thumbnailsToCreate.takeFirst();
            locker.unlock();

            QString thumbnailPath = m_document->getThumbnailPath(page);
            if (QFile::exists(thumbnailPath)) {
                qDebug() << "thumbnail" << thumbnailPath << "already exists!";
                continue;
            }

            QImage image = m_pdfWorker->loadOriginalPage(page, QSize(Settings::thumbnailWidth(), Settings::thumbnailHeight()));
            image.save(thumbnailPath);
            emit thumbnailUpdated(page);
        }
    }
}

void DocumentWorker::onPageLoaded(int page, QImage contents)
{
    if (page == m_currentPage) {
        if (!pageContents.isNull()) {
            return;
        }
        pageContents = contents;
        emit currentPageLoaded();
        return;
    }

    m_cachedContents[page] = contents;
}

void DocumentWorker::onBackgroundLoaded(int page, QImage contents)
{
    m_cachedBackgrounds[page] = contents;

    if (page == m_currentPage && !pageDirty) {
        emit currentPageLoaded();
    }
}

void DocumentWorker::preload()
{
    QMutexLocker locker(&m_lock);

    if (!m_cachedContents.contains(m_currentPage)) {
        m_pagesToLoad.insert(m_currentPage, m_document->getStoredPagePath(m_currentPage));
    }

    if (m_pdfWorker && !m_cachedBackgrounds.contains(m_currentPage) && !m_backgroundsToLoad.contains(m_currentPage)) {
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

void DocumentWorker::deletePages(QList<int> pagesToRemove)
{
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

    QVector<QVector<Line>> newLines;
    int pagesTaken = 0;
    for (int oldPage : oldPages.keys()) {
        if (pagesToRemove.contains(oldPage)) {
            QFile::remove(m_document->getThumbnailPath(oldPage));
            continue;
        }

        QString oldPath(m_document->getThumbnailPath(oldPages[oldPage]));
        QString newPath(m_document->getThumbnailPath(pagesTaken));
        QFile::rename(oldPath, newPath);
        newLines.append(m_lines[oldPage]);

        pagesTaken++;
    }

    m_lines = newLines;
}

void DocumentWorker::onMissingThumbnailRequested(int page)
{
    QMutexLocker locker(&m_lock);
    if (!m_thumbnailsToCreate.contains(page)) {
        m_thumbnailsToCreate.append(page);
    }
    m_waitCondition.wakeAll();
}
