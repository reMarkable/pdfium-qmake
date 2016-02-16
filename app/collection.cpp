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

#define RECENTLY_USED_KEY "RecentlyUsed"

Collection::Collection(QObject *parent) : QObject(parent)
{
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
            paths.append(file.absoluteFilePath());
        }

        if (file.isDir()) {
            paths.append(file.absoluteFilePath());
        }
    }

    return paths;
}

bool Collection::isFolder(const QString &path) const
{
    return !QFile::exists(path + ".metadata");
}

QObject *Collection::getDocument(const QString &path)
{
    QFileInfo pathInfo(path);

    QObject *document = nullptr;

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
    QQmlEngine::setObjectOwnership(document, QQmlEngine::JavaScriptOwnership);
    return document;
}

QObject *Collection::createDocument(const QString &defaultTemplate)
{
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

    return document;
}

QStringList Collection::recentlyUsedPaths(int count, int offset) const
{
    QSettings settings;
    QStringList recentlyUsed = settings.value(RECENTLY_USED_KEY).toStringList();

    if (!recentlyUsed.isEmpty()) {
        return recentlyUsed;
    }

    QDir dir(collectionPath() + "/Local/");

    QFileInfoList fileList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);

    QStringList paths;
    for (int i=offset; i<qMin(fileList.count(), count + offset); i++) {
        paths.append(fileList[i].canonicalFilePath());
    }

    return paths;
}

int Collection::localDocumentCount()
{
    QDir dir(collectionPath() + "/Local/");
    return dir.entryList(QDir::Dirs).count();
}

QStringList Collection::recentlyImportedPaths(int count) const
{
    QDir dir(collectionPath() + "/Dropbox/");

    QFileInfoList fileList = dir.entryInfoList(QDir::Files, QDir::Time);

    QStringList paths;
    for (int i=0; i<qMin(fileList.count(), count); i++) {
        paths.append(fileList[i].canonicalFilePath());
    }

    return paths;
}

QString Collection::thumbnailPath(const QString &documentPath) const
{
    QString cachedPath(documentPath + ".thumbnail.jpg");
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
    //QDir dir(documentPath);
    //QString title = dir.dirName();
    //
    return QFileInfo(documentPath).fileName();
}

int Collection::pageCount(const QString documentPath) const
{
    return documentPath.length() % 13;
}
