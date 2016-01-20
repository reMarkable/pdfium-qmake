#include "collection.h"

#include "pdfdocument.h"
#include "imagedocument.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QQmlEngine>

#define RECENTLY_USED_KEY "RecentlyUsed"

static const char *s_fontpaths[] = { "/system/fonts/", nullptr };

Collection::Collection(QObject *parent) : QObject(parent)
{
#ifdef Q_PROCESSOR_ARM
    m_basePath = "/data/documents/";
#else// Q_PROCESSOR_ARM
    m_basePath = "/home/sandsmark/xo/testdata";
#endif// Q_PROCESSOR_ARM

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), &m_imageloadingThread, SLOT(quit()));
    m_imageloadingThread.start(QThread::LowestPriority);

    FPDF_LIBRARY_CONFIG_ config;
    config.version = 2;
    config.m_pUserFontPaths = s_fontpaths;
    config.m_pIsolate = nullptr;
    config.m_v8EmbedderSlot = 0;
    FPDF_InitLibraryWithConfig(&config);
}

Collection::~Collection()
{
    FPDF_DestroyLibrary();
}

QStringList Collection::folderEntries(QString path) const
{
    if (path.isEmpty()) {
        path = m_basePath;
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

    if (!pathInfo.exists(path)) {
        qWarning() << "Asked for non-existing path";
        return nullptr;
    }

    Document *document = nullptr;
    if (pathInfo.isFile() && path.endsWith(".pdf")) {
        document = new PdfDocument(path);
    } else if (pathInfo.isDir()){
        document = new ImageDocument(path);
    } else {
        qWarning() << "Asked for invalid path" << path;
    }

    document->moveToThread(&m_imageloadingThread);
    QTimer::singleShot(10, document, SLOT(preload()));
    QQmlEngine::setObjectOwnership(document, QQmlEngine::JavaScriptOwnership);
    return document;
}

QStringList Collection::recentlyUsedPaths() const
{
    QSettings settings;
    QStringList recentlyUsed = settings.value(RECENTLY_USED_KEY).toStringList();

    if (recentlyUsed.isEmpty()) {
        return QStringList() << m_basePath + "/Local/dijkstra.pdf"
                             //<< m_basePath + "/Local/jantu.pdf"
                             //<< m_basePath + "/Dropbox/images.zip"
                             << m_basePath + "/Local/imx.pdf"
                             << m_basePath + "/Dropbox/master.pdf";
    } else {
        return recentlyUsed;
    }
}

QString Collection::thumbnailPath(const QString &documentPath) const
{
    QString cachedPath(documentPath + ".cached.jpg");
    if (QFile::exists(cachedPath)) {
        return cachedPath;
    }

    QDir dir(documentPath);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.png", QDir::Files, QDir::Name);
    if (fileList.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "No images in path" << documentPath;
        return QString();
    }

    return fileList.first().absoluteFilePath();
}

QString Collection::title(const QString &documentPath) const
{
    QDir dir(documentPath);
    return dir.dirName();
}
