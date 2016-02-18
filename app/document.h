#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "line.h"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QImage>
#include <QMutex>
#include <QSize>
#include <QThread>

class Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)


public:
    explicit Document(QString path, QObject *parent = 0);
    virtual ~Document();

    QImage background();
    const QVector<Line> &lines();
    void addLine(Line line);
    Line popLine();

    void setDrawnPage(const QImage &pageContents);

public slots:
    void setCurrentIndex(int index);

    int pageCount() { return m_pageCount; }

    /// We should preload pages into our cache
    void preload();

    /// We should clear our cache
    void clearCache();

    int currentIndex() const { return m_currentIndex; }
    QString path() { return m_path; }

    virtual void setTemplate(QString backgroundTemplate) = 0;
    virtual QStringList availableTemplates() const = 0;
    virtual QString currentTemplate() const = 0;

signals:
    void currentIndexChanged();
    void pageCountChanged();
    void backgroundChanged();
    void pageRequested(int index);
    void storingRequested(QImage image, int index);
    void templateChanged();

protected slots:
    virtual QImage loadOriginalPage(int index, QSize dimensions) = 0;
    void setPageCount(int pageCount);
    void setCurrentBackground(QImage background);

private:
    friend class DocumentWorker;
    void loadPage(int index);
    void storePage(QImage image, int index);

    QImage getStoredPage(int index);
    inline QString getStoredPagePath(int index) {
        return m_path + '-' + QString::number(index) + ".cached.png";
    }

    inline QString getThumbnailPath(int index) {
        return m_path + '-' + QString::number(index) + ".thumbnail.jpg";
    }

    QString m_path;
    int m_currentIndex;
    int m_pageCount;
    QHash<int, QVector<Line>> m_lines;
    QHash<int, QImage> m_cachedBackgrounds;
    QHash<int, QImage> m_pageContents;
    QMutex m_cacheLock;
    bool m_pageDirty;
    QThread m_workerThread;
};

// To ensure that things are called from another thread
class DocumentWorker : public QObject
{
    Q_OBJECT
public:
    DocumentWorker(Document *document) : QObject(), m_document(document)
    {
    }

private slots:
    // These will be asynchronously called on this object's thread,
    // and synchronously invoke the methods on the Document object
    void onPageRequested(int index) {
        m_document->loadPage(index);
    }
    void onStoringRequested(QImage image, int index) {
        m_document->storePage(image, index);
    }

private:
    Document *m_document;
};

#endif // DOCUMENT_H
