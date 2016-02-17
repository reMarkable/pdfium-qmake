#include "document.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>
#include <QFile>

#define CACHE_COUNT 5 // Cache up to 5 page before and after

#define DEBUG_THIS
#include "debug.h"

Document::Document(QString path, QObject *parent)
    : QObject(parent),
      m_path(path),
      m_currentIndex(0),
      m_pageCount(1),
      m_pageDirty(false)
{
    DEBUG_BLOCK;

    // Set up a worker to let stuff be loaded in another thread
    DocumentWorker *worker = new DocumentWorker(this);
    worker->moveToThread(&m_workerThread);
    m_workerThread.start(QThread::LowestPriority);

    connect(&m_workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(pageRequested(int)), worker, SLOT(onPageRequested(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(storingRequested(QImage,int)), worker, SLOT(onStoringRequested(QImage,int)), Qt::QueuedConnection);

    QFile metadataFile(path + ".metadata");
    if (metadataFile.open(QIODevice::ReadOnly)) {
        m_currentIndex = metadataFile.readLine().trimmed().toInt();
        m_pageCount = metadataFile.readLine().trimmed().toInt();
    }

    QString cachedBackgroundPath = getStoredPagePath(m_currentIndex);
    if (QFile::exists(cachedBackgroundPath)) {
        m_pageContents[m_currentIndex] = QImage(cachedBackgroundPath);
    }
}

Document::~Document()
{
    DEBUG_BLOCK;

    m_workerThread.quit();
    m_workerThread.wait(); // don't destroy while twerking

    QFile metadataFile(m_path + ".metadata");
    if (metadataFile.open(QIODevice::WriteOnly)) {
        metadataFile.write(QByteArray::number(m_currentIndex) + "\n");
        metadataFile.write(QByteArray::number(m_pageCount) + "\n");
    }

    QMutexLocker locker(&m_cacheLock);

    if (m_pageContents.contains(m_currentIndex)) {
        storePage(m_pageContents[m_currentIndex], m_currentIndex);
        m_pageContents[m_currentIndex].save(m_path + ".thumbnail.jpg");
    } else if (m_cachedBackgrounds.contains(m_currentIndex)){
        m_cachedBackgrounds[m_currentIndex].save(m_path + ".thumbnail.jpg");
    }
}

QImage Document::background()
{
    QMutexLocker locker(&m_cacheLock);

    if (m_pageContents.contains(m_currentIndex)) {
        return m_pageContents[m_currentIndex];
    }
    if (m_cachedBackgrounds.contains(m_currentIndex)) {
        return m_cachedBackgrounds[m_currentIndex];
    }

    // It will hopefully be returned soon
    return QImage();
}

const QVector<Line> &Document::lines()
{
    return m_lines[m_currentIndex];
}

void Document::addLine(Line line)
{
    m_lines[m_currentIndex].append(line);
}

Line Document::popLine()
{
    if (!m_lines.contains(m_currentIndex)) {
        return Line();
    }

    return m_lines[m_currentIndex].takeLast();
}

void Document::setDrawnPage(const QImage &pageContents)
{
    QMutexLocker locker(&m_cacheLock);
    m_pageDirty = true;
    if (pageContents.isNull()) {
        m_pageContents.remove(m_currentIndex);
    } else {
        m_pageContents[m_currentIndex] = pageContents;
    }
}

void Document::setCurrentIndex(int newIndex)
{
    if (m_currentIndex == newIndex || newIndex < 0) {
        return;
    }

    if (newIndex >= m_pageCount) {
        return;
    }

    // If we have drawn on the current page, we need to store it
    if (m_pageDirty) {
        emit storingRequested(m_pageContents.value(m_currentIndex), m_currentIndex);
        m_pageDirty = false;
    }

    const int cacheMin = qMax(newIndex - CACHE_COUNT, 0);
    const int cacheMax = newIndex + CACHE_COUNT;

    QMutexLocker locker(&m_cacheLock);
    foreach(int index, m_cachedBackgrounds.keys()) {
        if (index > cacheMin || index < cacheMax) {
            continue;
        }

        m_cachedBackgrounds.remove(index);
    }

    m_currentIndex = newIndex;

    locker.unlock();

    preload();

    emit currentIndexChanged();
}

void Document::preload()
{
    QMutexLocker locker(&m_cacheLock);

    if (!m_cachedBackgrounds.contains(m_currentIndex)) {
        emit pageRequested(m_currentIndex);
    }

    for (int i=1; i<CACHE_COUNT; i++) {
        const int indexForward = m_currentIndex + i;
        const int indexBackward = m_currentIndex - i;

        if (!m_cachedBackgrounds.contains(indexForward)) {
            emit pageRequested(indexForward);
        }

        if (!m_cachedBackgrounds.contains(indexBackward)) {
            emit pageRequested(indexBackward);
        }
    }
}

void Document::clearCache()
{
    QMutexLocker locker(&m_cacheLock);

    foreach(int index, m_cachedBackgrounds.keys()) {
        // Only keep the current page
        if (index != m_currentIndex) {
            m_cachedBackgrounds.remove(index);
        }
    }

    foreach(int index, m_pageContents.keys()) {
        // Only keep the current page
        if (index != m_currentIndex) {
            m_pageContents.remove(index);
        }
    }
}

void Document::setPageCount(int pageCount)
{
    if (pageCount == m_pageCount) {
        return;
    }

    m_pageCount = pageCount;
    emit pageCountChanged();
}

void Document::setCurrentBackground(QImage background)
{
    QMutexLocker locker(&m_cacheLock);
    if (m_cachedBackgrounds.contains(m_currentIndex)) {
        m_cachedBackgrounds.remove(m_currentIndex);
    }
    if (m_pageContents.contains(m_currentIndex)) {
        m_pageContents.remove(m_currentIndex);
    }
    m_cachedBackgrounds[m_currentIndex] = background;
    locker.unlock();
    emit backgroundChanged();
}

void Document::loadPage(int index)
{
    QMutexLocker locker(&m_cacheLock);

    const QString drawnPagePath = getStoredPagePath(index);
    if (QFile::exists(drawnPagePath) && !m_pageContents.contains(index)) {
        locker.unlock();

        QImage page = QImage(drawnPagePath);

        locker.relock();

        m_pageContents[index] = page;

        if (index == m_currentIndex) {
            emit backgroundChanged();
        }
    }

    if (!m_cachedBackgrounds.contains(index)) {
        locker.unlock();
        QImage page = loadOriginalPage(index, QSize(1200, 1560));
        if (page.isNull()) {
            return;
        }

        locker.relock();

        m_cachedBackgrounds[index] = page;

        const QString thumbnailPath = getThumbnailPath(index);
        if (!QFile::exists(thumbnailPath)) {
            page.scaledToHeight(512).save(thumbnailPath);
        }


        if (index == m_currentIndex && !m_pageContents.contains(index)) {
            emit backgroundChanged();
        }
    }
}

void Document::storePage(QImage image, int index)
{
    QString filePath = getStoredPagePath(index);

    if (image.isNull()) {
        if (QFileInfo::exists(filePath)) {
            QFile::remove(filePath);
        }
    } else {
        image.save(filePath);        
        image.scaledToHeight(512).save(getThumbnailPath(index));
    }    
}
