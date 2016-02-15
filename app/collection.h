#ifndef COLLECTION_H
#define COLLECTION_H

#include <QObject>

class Collection : public QObject
{
    Q_OBJECT

public:
    explicit Collection(QObject *parent = 0);
    ~Collection();

    static QString collectionPath();

public slots:
    QStringList folderEntries(QString path = QString()) const;
    bool isFolder(const QString &path) const;
    QObject *getDocument(const QString &path);
    QObject *createDocument(const QString &defaultTemplate);
    QStringList recentlyUsedPaths(int count) const;
    QString thumbnailPath(const QString &documentPath) const;
    QString title(const QString &documentPath) const;
    int pageCount(const QString documentPath) const;

signals:

private:
};

#endif // COLLECTION_H
