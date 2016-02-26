#ifndef COLLECTION_H
#define COLLECTION_H

#include "document.h"

#include <QObject>
#include <QHash>
#include <QPointer>

class DocumentWorker;

class Collection : public QObject
{
    Q_OBJECT

public:
    explicit Collection(QObject *parent = 0);
    ~Collection();

    static QString collectionPath();

public slots:
    QObject *getDocument(const QString &path);
    QString createDocument(const QString &defaultTemplate);
    void deleteDocument(const QString documentPath);

    QStringList getDocumentPaths(int count, int offset = 0) const;
    QStringList getFrequentlyOpenedPaths(int count, int offset = 0) const;

    int documentCount();

    QString defaultDocumentPath(const QString &type) const;

    void archiveBookOpened(const QString path);
    void archiveBookClosed();

signals:
    void documentPathsChanged();

private:
    void loadDocument(const QString path);
    bool initializePDFDocument(Document *document);

    QHash<QString, QPointer<Document>> m_documents;
    QStringList m_sortedPaths;
    QPointer<DocumentWorker> m_archiveBookWorker;
};

#endif // COLLECTION_H
