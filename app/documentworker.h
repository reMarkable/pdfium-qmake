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

class PdfRenderer;

/*!
 * \class DocumentWorker
 *
 * \brief A class to handle loading and storing page contents, backgrounds and thumbnails.
 *
 * It holds separate FIFO queues for page contents to load, page contents to store,
 * backgrounds to store and thumbnails to store.
 *
 * It runs the loading and storing in its own thread, but its QObject lives in the default thread
 * so that signal handling doesn't block on the main loop in it.
 *
 * The main loop in the thread processes the queues, emptying the queues in order of importance.
 * When all the queues are empty, it waits on a QWaitCondition.
 *
 * The calls in the QObject thread appends to the appropriate queue, and then calls wakeAll() on
 * the QWaitCondition to ensure that the background thread loop is running.
 *
 * The suspend() and wake() are used to suspend the loading and storing to ensure that the drawing
 * in the DrawingArea doesn't get interrupted.
 *
 * It shouldn't be deleted manually, it deletes itself when the thread ends (i. e.
 * after stop() is called).
 */

class DocumentWorker : public QThread
{
    Q_OBJECT
public:
    DocumentWorker(Document *document);

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

    void storeThumbnail();

    void stop();

signals:
    void backgroundsLoaded(int page, QImage contents);
    void pageLoaded(int page, QImage contents);
    void updateNeeded();
    void thumbnailUpdated(int page);

    void redrawingNeeded();

protected:
    virtual void run() override;

private slots:
    void onPageLoaded(int page, QImage contents);
    void onBackgroundLoaded(int page, QImage contents);

    void onPageChanged(int newPage);
    void deletePages(QList<int> pagesToRemove);

    void onMissingThumbnailRequested(int page);

    void onTemplateChanged();

private:
    ~DocumentWorker();

    void loadLines();
    void storeLines();
    void printMemoryUsage() const;
    void clearLoadQueue();
    bool hasLinesForPage(int page);

    QPointer<Document> m_document;

    QVector<QVector<Line>> m_lines;
    QHash<int, QImage> m_cachedBackgrounds;
    QHash<int, QImage> m_cachedContents;

    QHash<int, QImage> m_pagesToStore;
    QList<int> m_backgroundsToLoad;
    QHash<int, QString> m_pagesToLoad;
    QHash<int, QImage> m_thumbnailsToCreate;

    QMutex m_lock;

    bool m_suspended;
    PdfRenderer *m_pdfRenderer;
    QWaitCondition m_waitCondition;
    QWaitCondition m_suspendCondition;
    QString m_currentTemplate;
    int m_currentPage;
};

#endif // DOCUMENTWORKER_H
