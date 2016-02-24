#ifndef COLLECTION_H
#define COLLECTION_H

#include "document.h"

#include <QObject>
#include <QMap>
#include <QPointer>

class Collection : public QObject
{
    Q_OBJECT

public:
    explicit Collection(QObject *parent = 0);
    ~Collection();

    static QString collectionPath();

public slots:
    QObject *getDocument(const QString &path);
    QObject *createDocument(const QString &defaultTemplate);
    QStringList getDocumentPaths(int count, int offset = 0) const;

    int documentCount();

    QString thumbnailPath(const QString &documentPath) const;
    QString title(const QString &documentPath) const;
    int pageCount(const QString documentPath) const;
    void deleteDocument(const QString documentPath);

signals:
    void documentPathsChanged();

private:
    QMap<QString, QPointer<Document>> m_openDocuments;
    QMap<QString, int> m_documentsPageCount;
    QMap<QString, int> m_documentsLastPage;
    QStringList m_documentPaths;
};

#endif // COLLECTION_H
