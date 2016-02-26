#ifndef DOCUMENTWORKER_H
#define DOCUMENTWORKER_H

#include "document.h"
#include "line.h"

#include <QObject>
#include <QPointer>
#include <QThread>
#include <QHash>
#include <QVector>
#include <QSet>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>

class PDFWorker;

// To ensure that things are called from another thread
class DocumentWorker : public QThread
{
    Q_OBJECT
public:
    DocumentWorker(Document *document);
    ~DocumentWorker();

    void suspend();
    void wake();

    QImage background();

    /// We should preload pages into our cache
    void preload();

    /// We should clear our cache
    void pruneCache();

    const QVector<Line> &lines();
    void addLine(Line line);
    Line popLine();

    QImage pageContents;
    bool pageDirty;

protected:
    virtual void run() override;


signals:
    void backgroundsLoaded(int page, QImage contents);
    void pageLoaded(int page, QImage contents);
    void currentPageLoaded();
    void thumbnailUpdated(int page);

private slots:
    void onPageLoaded(int page, QImage contents);
    void onBackgroundLoaded(int page, QImage contents);

    void onPageChanged(int newPage);
    void deletePages(QList<int> pagesToRemove);

    void onMissingThumbnailRequested(int page);

private:
    void storeLines();
    void printMemoryUsage() const;
    void clearLoadQueue();

    QPointer<Document> m_document;

    QVector<QVector<Line>> m_lines;
    QHash<int, QImage> m_cachedBackgrounds;
    QHash<int, QImage> m_cachedContents;

    QHash<int, QImage> m_pagesToStore;
    QList<int> m_backgroundsToLoad;
    QHash<int, QString> m_pagesToLoad;
    QList<int> m_thumbnailsToCreate;

    QMutex m_lock;

    bool m_suspended;
    PDFWorker *m_pdfWorker;
    QWaitCondition m_waitCondition;
    QWaitCondition m_suspendCondition;
    int m_currentPage;
};

#endif // DOCUMENTWORKER_H
