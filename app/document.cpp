#include "document.h"

#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>
#include <QFile>

#define CACHE_COUNT 5 // Cache up to 5 page before and after

Document::Document(QString path, QObject *parent)
    : QObject(parent),
      m_path(path),
      m_currentIndex(0),
      m_pageCount(1),
      m_dimensions(1200, 1560),
      m_pageDirty(false)
{
    connect(this, SIGNAL(backgroundLoaded(QImage,int)), SLOT(cacheBackground(QImage,int)));
    connect(this, SIGNAL(pageRequested(int)), SLOT(loadPage(int)), Qt::QueuedConnection);

    QFile metadataFile(path + ".metadata");
    if (metadataFile.open(QIODevice::ReadOnly)) {
        int lastPageOpen = metadataFile.readLine().toInt();
        if (lastPageOpen > 0) {
            m_currentIndex = lastPageOpen;
        }
    }

    QString cachedBackgroundPath = getStoredPagePath(m_currentIndex);
    if (QFile::exists(cachedBackgroundPath)) {
        m_cachedBackgrounds[m_currentIndex] = QImage(cachedBackgroundPath);
    }
}

Document::~Document()
{
    QFile metadataFile(m_path + ".metadata");
    if (metadataFile.open(QIODevice::WriteOnly)) {
        metadataFile.write(QByteArray::number(m_currentIndex) + "\n");
    }

    // If we have drawn on the current page, we need to store it
    storeDrawnPage();

    QMutexLocker locker(&m_cacheLock);
    if (m_cachedBackgrounds.contains(m_currentIndex)) {
        m_cachedBackgrounds[m_currentIndex].save(m_path + ".cached.jpg");
    }
}

QImage Document::background()
{
    QMutexLocker locker(&m_cacheLock);

    if (!m_cachedBackgrounds.contains(m_currentIndex)) { // it will be loaded soon
        return QImage();
    }

    return m_cachedBackgrounds[m_currentIndex];
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
        QString filePath = getStoredPagePath(m_currentIndex);

        if (!m_cachedBackgrounds.contains(m_currentIndex)) {
            if (QFileInfo::exists(filePath)) {
                QFile::remove(filePath);
            }
        }

        m_cachedBackgrounds.remove(m_currentIndex);
        emit pageRequested(m_currentIndex);
    } else {
        m_cachedBackgrounds[m_currentIndex] = pageContents;
    }
}

void Document::setCurrentIndex(int newIndex)
{
    if (m_currentIndex == newIndex || newIndex < 0) {
        return;
    }

    QMutexLocker locker(&m_cacheLock);

    if (newIndex >= m_pageCount) {
        return;
    }

    // If we have drawn on the current page, we need to store it
    if (m_pageDirty) {
        storeDrawnPage();
        m_pageDirty = false;
    }

    const int cacheMin = qMax(newIndex - CACHE_COUNT, 0);
    const int cacheMax = newIndex + CACHE_COUNT;

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

void Document::setDimensions(QSize size)
{
    QMutexLocker locker(&m_cacheLock);

    if (size == m_dimensions) {
        return;
    }

    m_cachedBackgrounds.clear();
    setCurrentIndex(m_currentIndex); // force reload
}

void Document::preload()
{
    QMutexLocker locker(&m_cacheLock);

    if (!m_cachedBackgrounds.contains(m_currentIndex)) {
        emit pageRequested(m_currentIndex);
    }

    for (int i=1; i<CACHE_COUNT; i++) {
        const int indexForward = m_currentIndex + i;
        const int indexBackward = m_currentIndex + i;

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
    foreach(int index, m_cachedBackgrounds.keys()) {
        // Only keep the current page
        if (index != m_currentIndex) {
            m_cachedBackgrounds.remove(index);
        }
    }
}

void Document::cacheBackground(QImage image, int index)
{
    QMutexLocker locker(&m_cacheLock);

    if (m_cachedBackgrounds.contains(index)) {
        qWarning() << "Trying to cache already cached background, index:" << index;
    }

    m_cachedBackgrounds[index] = image;

    if (index == m_currentIndex) {
        locker.unlock();
        emit backgroundChanged();
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

QList<int> Document::cachedIndices()
{
    QMutexLocker locker(&m_cacheLock);

    return m_cachedBackgrounds.keys();
}

void Document::storeDrawnPage()
{
    QString filePath = getStoredPagePath(m_currentIndex);

    if (!m_cachedBackgrounds.contains(m_currentIndex)) {
        if (QFileInfo::exists(filePath)) {
            QFile::remove(filePath);
        }

        return;
    }

    m_cachedBackgrounds[m_currentIndex].save(filePath);
}

void Document::loadPage(int index)
{
    { // Check if it is already cached
        QMutexLocker locker(&m_cacheLock);

        if (m_cachedBackgrounds.contains(index)) {
            return;
        }
    }

    QString cachedBackgroundPath = getStoredPagePath(index);
    if (QFile::exists(cachedBackgroundPath)) {
        cacheBackground(QImage(getStoredPagePath(index)), index);
    } else {
        loadOriginalPage(index);
    }
}
