#include "collection.h"

#include "documentworker.h"
#include "pdfrenderer.h"
#include "settings.h"

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

    QString localPath = collectionPath() + "/Local/";
    if (!QFile::exists(localPath)) {
        QDir(localPath).mkpath(localPath);
    }
    QString importedPath = collectionPath() + "/Imported/";
    if (!QFile::exists(importedPath)) {
        QDir(importedPath).mkpath(importedPath);
    }

    dir.setPath(localPath);
    fileList.append(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
    dir.setPath(importedPath);
    fileList.append(dir.entryInfoList(QStringList() << "*.pdf", QDir::Files));

    std::sort(fileList.begin(), fileList.end(), [](const QFileInfo &a, const QFileInfo &b) {
        return a.created() > b.created();
    });

    for (const QFileInfo fileInfo : fileList) {
        QString documentPath = fileInfo.canonicalFilePath();
        if (!documentPath.startsWith(collectionPath()) || !QFile::exists(documentPath)) {
            continue;
        }

        loadDocument(documentPath);
        m_sortedPaths.append(documentPath);
    }

    QString defaultPath = defaultDocumentPath("Sketch");
    if (!QFile::exists(defaultPath)) {
        if (!Document::createDocument("Sketch", defaultPath)) {
            qWarning() << "Unable to create default sketch document";
        }
    }
    loadDocument(defaultPath);

    defaultPath = defaultDocumentPath("Lined");
    if (!QFile::exists(defaultPath)) {
        if (!Document::createDocument("Lined", defaultPath)) {
            qWarning() << "Unable to create default note document";
        }
    }
    loadDocument(defaultPath);
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

    if (!m_documents.contains(path)) {
        qWarning() << "Asked for invalid document" << path;
        qDebug() << m_documents;
        return nullptr;
    }

    return m_documents.value(path).data();
}

QString Collection::createDocument(const QString &defaultTemplate)
{
    DEBUG_BLOCK;

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

    if (!Document::createDocument(defaultTemplate, path)) {
        qWarning() << "Unable to create document";
        return QString();
    }

    loadDocument(path);
    m_sortedPaths.prepend(path);

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
    paths.append(m_sortedPaths.mid(offset, count));

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

    QList<QPointer<Document>> documents;
    for (const QString &path : m_documents.keys()) {
        if (path == defaultDocumentPath("Sketch") || path == defaultDocumentPath("Lined")) {
            continue;
        }
        documents.append(m_documents.value(path));
    }

    std::sort(documents.begin(), documents.end(), [](const QPointer<Document> a, const QPointer<Document> b) {
        return a->openCount() > b->openCount();
    });

    count = qMin(documents.count(), count);
    for (int i=0; i<count; i++) {
        sortedPaths.append(documents[i]->path());
    }

    return sortedPaths;
}

int Collection::documentCount()
{
    DEBUG_BLOCK;

    return m_sortedPaths.count();
}

void Collection::deleteDocument(const QString documentPath)
{
    if (!documentPath.startsWith(collectionPath())) {
        qWarning() << Q_FUNC_INFO << "Asked to delete invalid path" << documentPath;
        return;
    }

    if (!m_documents.contains(documentPath)) {
        qWarning() << "Trying to delete unknown path";
    }

    QStringList filesToDelete;
    filesToDelete << documentPath + ".metadata"
                  << documentPath + ".pagedata";

    Document *document = m_documents.value(documentPath);

    for (int i=0; i<document->pageCount(); i++) {
        QString thumbnailPath = document->getThumbnailPath(i);
        filesToDelete.append(thumbnailPath);
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

    m_sortedPaths.removeAll(documentPath);
    m_documents.take(documentPath)->deleteLater();

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

void Collection::archiveBookOpened(const QString path)
{
    if (!m_documents.contains(path)) {
        qWarning() << "Invalid book opened in archive";
    }
    if (m_archiveBookWorker)  {
        m_archiveBookWorker->stop();
    }
    m_archiveBookWorker = new DocumentWorker(m_documents.value(path));
    m_archiveBookWorker->start();
}

void Collection::archiveBookClosed()
{
    if (m_archiveBookWorker)  {
        m_archiveBookWorker->stop();
    }
}

void Collection::loadDocument(const QString path)
{
    if (!QFile::exists(path)) {
        qWarning() << "Asked to load non-existing document";
        return;
    }

    Document *document = new Document(path);
    if (path.endsWith(".pdf")) {
        initializePDFDocument(document);
    }
    QQmlEngine::setObjectOwnership(document, QQmlEngine::CppOwnership);
    m_documents.insert(path, QPointer<Document>(document));
}

bool Collection::initializePDFDocument(Document *document)
{
    if (!document) {
        qWarning() << "Got null document";
        return false;
    }

    int page = document->currentPage();
    QString thumbnailPath = document->getThumbnailPath(page);
    if (QFile::exists(thumbnailPath)) { // This document has already been initialized
        return true;
    }

    PdfRenderer renderer(document);
    if (!renderer.initialize()) {
        qWarning() << "Can't initialize PDF worker for" << document->path();
        return false;
    }

    QImage thumbnail = renderer.renderPage(page, QSize(Settings::thumbnailWidth(), Settings::thumbnailHeight()));
    if (thumbnail.isNull()) {
        qWarning() << "Unable to load PDF page" << page << "from" << document->path();
    }

    thumbnail.save(document->getThumbnailPath(page));

    return true;
}
