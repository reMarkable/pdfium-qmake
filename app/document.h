#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "line.h"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QImage>
#include <QMutex>
#include <QSize>

class Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex MEMBER m_currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
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

    void setDimensions(QSize size);

    int pageCount() { return m_pageCount; }

    /// We should preload pages into our cache
    void preload();

    /// We should clear our cache
    void clearCache();

signals:
    void currentIndexChanged();
    void pageCountChanged();
    void backgroundChanged();
    void pageRequested(int index);
    void storingRequested(QImage image, int index);

protected slots:
    virtual QImage loadOriginalPage(int index) = 0;
    void setPageCount(int pageCount);
    QSize dimensions() { return m_dimensions; }
    QString path() { return m_path; }

private slots:
    void loadPage(int index);
    void storePage(QImage image, int index);

private:
    QImage getStoredPage(int index);
    inline QString getStoredPagePath(int index) {
        return m_path + '-' + QString::number(index) + ".cached.jpg";
    }

    QString m_path;
    int m_currentIndex;
    int m_pageCount;
    QHash<int, QVector<Line>> m_lines;
    QHash<int, QImage> m_cachedBackgrounds;
    QHash<int, QImage> m_pageContents;
    QMutex m_cacheLock;
    QSize m_dimensions;
    bool m_pageDirty;
};

#endif // DOCUMENT_H
