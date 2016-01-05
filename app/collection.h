#ifndef COLLECTION_H
#define COLLECTION_H

#include <QObject>
#include <QThread>
#include "page.h"

class Collection : public QObject
{
    Q_OBJECT

public:
    explicit Collection(QObject *parent = 0);

public slots:
    QStringList folderEntries(QString path = QString()) const;
    bool isFolder(const QString &path, const QString &name) const;
    QList<QObject *> getPages(const QString &path);
    QStringList recentlyUsedPaths() const;
    QString thumbnailPath(const QString &documentPath) const;
    QString title(const QString &documentPath) const;

signals:

private:
    QString m_basePath;
    QThread m_imageloadingThread;
};

#endif // COLLECTION_H
