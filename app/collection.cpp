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
        if (!documentPath.startsWith(collectionPath()) || !QFile::exists(documentPath)) {
            continue;
        }

        m_documentPaths.append(documentPath);

        QFile metadataFile(documentPath + ".metadata");
        if (!metadataFile.open(QIODevice::ReadOnly)) {
            qWarning() << "unable to open metadata" << documentPath;
            continue;
        }

        m_documentsLastPage[documentPath] = metadataFile.readLine().trimmed().toInt();
        m_documentsPageCount[documentPath] = metadataFile.readLine().trimmed().toInt();
    }

    QFile openCountsFile(collectionPath() + "opencounts.data");
    if (openCountsFile.open(QIODevice::ReadOnly)) {
        while (!openCountsFile.atEnd()) {
            QByteArray line = openCountsFile.readLine().trimmed();
            int index = line.indexOf(' ');
            if (index == -1) {
                qWarning() << "Invalid line in" << openCountsFile.fileName() << ":" << line;
                continue;
            }

            QString path = QString::fromUtf8(line.mid(index)).trimmed();
            if (!m_documentPaths.contains(path)) {
                continue;
            }

            m_documentOpenCount.insert(path, line.left(index).toInt());
        }
    } else {
        qWarning() << "unable to open" << openCountsFile.fileName();
    }

    QString defaultPath = defaultDocumentPath("Sketch");
    if (!QFile::exists(defaultPath)) {
        QDir(defaultPath).mkpath(defaultPath);
    }

    defaultPath = defaultDocumentPath("Lined");
    if (!QFile::exists(defaultPath)) {
        QDir(defaultPath).mkpath(defaultPath);
    }
}

Collection::~Collection()
{
    storeMetadata();
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

    m_documentOpenCount[path]++;
    emit documentsOpenCountsChanged();

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
        qWarning() << "Document doesn't exist:" << path;
        return document;
    } else if (pathInfo.isFile() && path.endsWith(".pdf")) {
        document = new PdfDocument(path);
    } else if (pathInfo.isDir()) {
        qWarning() << "returning new document object";
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

QString Collection::createDocument(const QString &defaultTemplate)
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
        return QString();
    }

    if (!QDir(path).mkpath(path)) {
        qWarning() << "Unable to create document";
        return QString();
    }

    m_documentsLastPage.insert(path, 0);
    m_documentsPageCount.insert(path, 1);
    m_documentPaths.prepend(path);
    m_documentOpenCount.insert(path, 1);

    emit documentPathsChanged();

    return path;
}

QStringList Collection::getDocumentPaths(int count, int offset) const
{
    DEBUG_BLOCK;

    QStringList paths;
    if (offset == 0) {
        count -= 2; // For the default documents
        paths.append(defaultDocumentPath("Sketch"));
        paths.append(defaultDocumentPath("Lined"));
    } else {
        offset -= 2;
    }

    if (count < 1) {
        return paths;
    }
    paths.append(m_documentPaths.mid(offset, count));

    return paths;
}

QStringList Collection::getFrequentlyOpenedPaths(int count, int offset) const
{
    QStringList sortedPaths;
    if (offset == 0) {
        count -= 2; // For the default documents
        sortedPaths.append(collectionPath() + "Default notebook");
        sortedPaths.append(collectionPath() + "Default sketchbook");
    } else {
        offset -= 2;
    }

    if (count < 1) {
        return sortedPaths;
    }

    QMultiMap<int, QString> reverseMap;
    QMapIterator<QString, int> mapIterator(m_documentOpenCount);
    while (mapIterator.hasNext()) {
        mapIterator.next();
        reverseMap.insert(0 - mapIterator.value(), mapIterator.key());
    }

    sortedPaths.append(reverseMap.values().mid(offset, count));
    return sortedPaths;
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

QString Collection::defaultDocumentPath(const QString &type) const
{
    if (type == "Sketch") {
        return collectionPath() + "Default sketchbook";
    } else {
        return collectionPath() + "Default notebook";
    }
}

void Collection::storeMetadata()
{
    QFile openCountsFile(collectionPath() + "opencounts.data");
    if (!openCountsFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to open" << openCountsFile.fileName() << "for writing";
        return;
    }
    QMapIterator<QString, int> mapIterator(m_documentOpenCount);
    while (mapIterator.hasNext()) {
        mapIterator.next();
        openCountsFile.write(QByteArray::number(mapIterator.value()) + ' ' + mapIterator.key().toUtf8() + '\n');
    }
}
