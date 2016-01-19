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
    void backgroundLoaded(QImage image, int index);

protected slots:
    virtual void loadBackground(int index) = 0;
    void cacheBackground(QImage image, int index);
    void setPageCount(int pageCount);
    QSize dimensions() { return m_dimensions; }
    QString path() { return m_path; }
    QList<int> cachedIndices();

private:
    QString m_path;
    int m_currentIndex;
    int m_pageCount;
    QHash<int, QVector<Line>> m_lines;
    QHash<int, QImage> m_cachedBackgrounds;
    QMutex m_cacheLock;
    QSize m_dimensions;
};

#endif // DOCUMENT_H
