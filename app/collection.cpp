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

#include <algorithm>

#define DEBUG_THIS
#include "debug.h"

Collection::Collection(QObject *parent) : QObject(parent)
{
    DEBUG_BLOCK;

    QDir dir;
    QFileInfoList fileList;

    dir.setPath(collectionPath() + "/Local/");
    fileList.append(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
    dir.setPath(collectionPath() + "/Imported/");
    fileList.append(dir.entryInfoList(QStringList() << "*.pdf", QDir::Files));

    std::sort(fileList.begin(), fileList.end(), [](const QFileInfo &a, const QFileInfo &b) {
        return a.created() > b.created();
    });

    for (const QFileInfo fileInfo : fileList) {
        QString documentPath = fileInfo.canonicalFilePath();
        m_documentPaths.append(documentPath);

        QFile metadataFile(documentPath + ".metadata");
        if (!metadataFile.open(QIODevice::ReadOnly)) {
            qWarning() << "unable to open metadata" << documentPath;
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
    m_documentPaths.prepend(path);
    m_openDocuments.insert(path, QPointer<Document>(document));

    emit documentPathsChanged();

    return document;
}

QStringList Collection::getDocumentPaths(int count, int offset) const
{
    DEBUG_BLOCK;

    return m_documentPaths.mid(offset, count);
}

int Collection::documentCount()
{
    DEBUG_BLOCK;

    return m_documentPaths.count();
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
    m_documentPaths.removeAll(documentPath);

    emit documentPathsChanged();
}
