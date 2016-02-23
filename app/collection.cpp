#include "collection.h"

#include "pdfdocument.h"
#include "imagedocument.h"
#include "nativedocument.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QDateTime>

#define DEBUG_THIS
#include "debug.h"

#define RECENTLY_USED_KEY "RecentlyUsed"

Collection::Collection(QObject *parent) : QObject(parent)
{
    DEBUG_BLOCK;

    QDir localDir(localCollectionPath());

    QFileInfoList fileList = localDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for (const QFileInfo fileInfo : fileList) {
        QString documentPath = fileInfo.canonicalFilePath();
        m_recentlyUsedPaths.append(documentPath);

        QFile metadataFile(documentPath + ".metadata");
        if (!metadataFile.open(QIODevice::ReadOnly)) {
            continue;
        }

        m_documentsLastPage[documentPath] = metadataFile.readLine().trimmed().toInt();
        m_documentsPageCount[documentPath] = metadataFile.readLine().trimmed().toInt();
    }
}

Collection::~Collection()
{
}

QString Collection::collectionPath()
{
#ifdef Q_PROCESSOR_ARM
    return "/data/documents/";
#else// Q_PROCESSOR_ARM
    return "/home/sandsmark/xo/testdata/";
#endif// Q_PROCESSOR_ARM
}

QString Collection::localCollectionPath()
{
    return collectionPath() + "/Local/";
}

QStringList Collection::folderEntries(QString path) const
{
    if (path.isEmpty()) {
        path = collectionPath() + "/Local/";
    }

    QDir dir(path);
    QFileInfoList files = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    QStringList paths;
    for (const QFileInfo &file : files) {
        if (file.isFile() && file.suffix() == "pdf") {
            paths.append(file.canonicalFilePath());
        }

        if (file.isDir()) {
            paths.append(file.canonicalFilePath());
        }
    }

    return paths;
}

bool Collection::isFolder(const QString &path) const
{
    DEBUG_BLOCK;

    return !QFile::exists(path + ".metadata");
}

QObject *Collection::getDocument(const QString &path)
{
    DEBUG_BLOCK;

    if (m_openDocuments.contains(path)) {
        Document *document = m_openDocuments.value(path).data();
        if (document) {
            return document;
        } else {
            qDebug() << "removing" << path;
            m_openDocuments.remove(path);
        }
    }

    QFileInfo pathInfo(path);

    Document *document = nullptr;

    if (!pathInfo.exists(path)) {
        return document;
    } else if (pathInfo.isFile() && path.endsWith(".pdf")) {
        document = new PdfDocument(path);
    } else if (pathInfo.isDir()) {
        document = new NativeDocument(path);
    } else {
        qWarning() << "Asked for invalid path" << path;
    }

    QTimer::singleShot(10, document, SLOT(preload()));
    QTimer::singleShot(10, document, SLOT(loadLines()));
    QQmlEngine::setObjectOwnership(document, QQmlEngine::JavaScriptOwnership);
    m_openDocuments.insert(path, QPointer<Document>(document));
    return document;
}

QObject *Collection::createDocument(const QString &defaultTemplate)
{
    DEBUG_BLOCK;

    if (defaultTemplate.isEmpty()) {
        return nullptr;
    }

    QString path;
    for (int i=0; i<1000; i++) {
        path = collectionPath() + "/Local/" + defaultTemplate + ' ' + QString::number(i);
        if (!QFile::exists(path)) {
            break;
        }
    }

    if (QFile::exists(path)) {
        qWarning() << "unable to create unique path";
        return nullptr;
    }

    if (!QDir(path).mkpath(path)) {
        qWarning() << "Unable to create document";
        return nullptr;
    }

    NativeDocument *document = new NativeDocument(path, defaultTemplate);
    document->preload();

    QQmlEngine::setObjectOwnership(document, QQmlEngine::JavaScriptOwnership);

    m_documentsLastPage.insert(path, 0);
    m_documentsPageCount.insert(path, 1);
    m_recentlyUsedPaths.append(path);
    m_openDocuments.insert(path, QPointer<Document>(document));

    emit recentlyUsedChanged();

    return document;
}

QStringList Collection::recentlyUsedPaths(int count, int offset) const
{
    DEBUG_BLOCK;

    return m_recentlyUsedPaths.mid(offset, count);
}

int Collection::localDocumentCount()
{
    DEBUG_BLOCK;

    return m_recentlyUsedPaths.count();
}

QStringList Collection::recentlyImportedPaths(int count) const
{
    DEBUG_BLOCK;

    QDir dir(collectionPath() + "/Dropbox/");

    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.pdf", QDir::Files, QDir::Time);

    QStringList paths;
    for (int i=0; i<qMin(fileList.count(), count); i++) {
        paths.append(fileList[i].canonicalFilePath());
    }

    return paths;
}

QString Collection::thumbnailPath(const QString &documentPath) const
{
    DEBUG_BLOCK;

    QString cachedPath(documentPath + '-' + QString::number(m_documentsLastPage.value(documentPath)) + ".thumbnail.jpg");
    if (QFile::exists(cachedPath)) {
        return "file://" + cachedPath;
    }

    QDir dir(documentPath);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.png" << "*.jpg", QDir::Files, QDir::Name);
    if (fileList.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "No images in path" << documentPath;
        return QString();
    }

    return "file://" + fileList.first().absoluteFilePath();
}

QString Collection::title(const QString &documentPath) const
{
    DEBUG_BLOCK;

    return QFileInfo(documentPath).completeBaseName();
}

int Collection::pageCount(const QString documentPath) const
{
    DEBUG_BLOCK;

    return m_documentsPageCount.value(documentPath);
}

void Collection::deleteDocument(const QString documentPath)
{
    if (!documentPath.startsWith(collectionPath())) {
        qWarning() << Q_FUNC_INFO << "Asked to delete invalid path" << documentPath;
        return;
    }

    QStringList filesToDelete;
    filesToDelete << documentPath + ".thumbnail.jpg"
                  << documentPath + ".metadata"
                  << documentPath + ".pagedata";

    for (int i=0; i<m_documentsPageCount.value(documentPath); i++) {
        QString thumbnailFile(documentPath + '-' + QString::number(i) + ".thumbnail.jpg");
        filesToDelete.append(thumbnailFile);
    }

    for (const QString filePath : filesToDelete) {
        if (!filePath.startsWith(collectionPath())) {
            qWarning() << Q_FUNC_INFO << "Trying to delete invalid path" << filePath;
            continue;
        }
        if (!QFile::remove(filePath)) {
            qWarning() << Q_FUNC_INFO << "Unable to delete" << filePath;
        }
    }

    if (!QDir(documentPath).rmdir(documentPath)) {
        qWarning() << Q_FUNC_INFO << "Unable to delete directory" << documentPath;
    }

    m_documentsLastPage.remove(documentPath);
    m_documentsPageCount.remove(documentPath);
    m_recentlyUsedPaths.removeAll(documentPath);

    emit recentlyUsedChanged();
}
