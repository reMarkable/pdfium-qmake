#ifndef COLLECTION_H
#define COLLECTION_H

#include <QObject>
#include <QMap>

class Collection : public QObject
{
    Q_OBJECT

public:
    explicit Collection(QObject *parent = 0);
    ~Collection();

    static QString collectionPath();
    static QString localCollectionPath();

public slots:
    QStringList folderEntries(QString path = QString()) const;
    bool isFolder(const QString &path) const;
    QObject *getDocument(const QString &path);
    QObject *createDocument(const QString &defaultTemplate);
    QStringList recentlyUsedPaths(int count, int offset = 0) const;
    int localDocumentCount();
    QStringList recentlyImportedPaths(int count) const;
    QString thumbnailPath(const QString &documentPath) const;
    QString title(const QString &documentPath) const;
    int pageCount(const QString documentPath) const;
    void deleteDocument(const QString documentPath);

signals:
    void recentlyUsedChanged();

private:
    QMap<QString, int> m_documentsPageCount;
    QMap<QString, int> m_documentsLastPage;
    QStringList m_recentlyUsedPaths;
};

#endif // COLLECTION_H
