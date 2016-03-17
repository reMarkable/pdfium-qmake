#include "document.h"
#include "collection.h"
#include "settings.h"
#include "documentworker.h"
#include <QImage>
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>
#include <QFile>
#include <QDir>

#define DEBUG_THIS
#include "debug.h"

Document::Document(QString path, QObject *parent)
    : QObject(parent),
      m_currentPage(0),
      m_pageCount(1),
      m_openCount(0),
      m_worker(nullptr),
      m_workerReferences(0)
{
    DEBUG_BLOCK;

    if (!path.endsWith(".pdf") && !path.endsWith('/')) {
        path += '/';
    }
    m_path = path;


    QFile metadataFile(getMetadataPath());
    if (metadataFile.open(QIODevice::ReadOnly)) {
        m_currentPage = metadataFile.readLine().trimmed().toInt();
        m_pageCount = metadataFile.readLine().trimmed().toInt();
        m_openCount = metadataFile.readLine().trimmed().toInt();
    }

    connect(this, SIGNAL(currentPageChanged(int)), this, SLOT(storeMetadata()));
    connect(this, SIGNAL(openCountChanged()), this, SLOT(storeMetadata()));
    connect(this, SIGNAL(pageCountChanged()), this, SLOT(storeMetadata()));
    connect(this, SIGNAL(pageCountChanged()), this, SLOT(storeTemplates()));
    connect(this, SIGNAL(templateChanged()), this, SLOT(storeTemplates()));

    // We don't need to load anything more for PDFs
    if (path.endsWith(".pdf")) {
        return;
    }

    QFile templatesFile(path + ".pagedata");
    if (templatesFile.open(QIODevice::ReadOnly)) {
        while (!templatesFile.atEnd()) {
            QString pageTemplate = templatesFile.readLine().trimmed();
            if (pageTemplate.isEmpty()) {
                break;
            }
            m_templates.append(pageTemplate);
        }
    }

    QString defaultTemplate;
    if (!m_templates.isEmpty()) {
        defaultTemplate = m_templates.last();
    }

    if (defaultTemplate.isEmpty()) {
        defaultTemplate = "Blank";
    }

    if (m_templates.count() < pageCount()) {
        for (int i=0; i < (pageCount() - m_templates.count()); i++) {
            m_templates.append(defaultTemplate);
        }
    }
}

Document::~Document()
{
    DEBUG_BLOCK;
}

void Document::addOpenCount()
{
    m_openCount++;
}

bool Document::createDocument(QString documentType, QString path)
{
    if (documentType.isEmpty()) {
        documentType = "Sketchbook";
    }

    if (!path.endsWith('/')) {
        path += '/';
    }

    if (!QDir(path).mkpath(path)) {
        qWarning() << "Unable to create document";
        return false;
    }

    QFile metadataFile(path + ".metadata");
    if (metadataFile.open(QIODevice::WriteOnly)) {
        metadataFile.write(QByteArray::number(0) + "\n");
        metadataFile.write(QByteArray::number(1) + "\n");
        metadataFile.write(QByteArray::number(0) + "\n");
        metadataFile.close();
    }

    QString defaultTemplate;
    if (documentType == "Sketchbook") {
        defaultTemplate = "Blank";
    } else {
        defaultTemplate = "Lined";
    }

    QFile templatesFile(path + ".pagedata");
    if (templatesFile.open(QIODevice::WriteOnly)) {
        templatesFile.write(defaultTemplate.toUtf8() + '\n');
        templatesFile.close();
    }

    QImage templateImage(Collection::collectionPath() + "/templates/" + defaultTemplate + ".png");
    if (!templateImage.isNull()) {
        QImage thumbnail = templateImage.scaled(Settings::thumbnailWidth(), Settings::thumbnailHeight());
        thumbnail.save(path + ".0.thumbnail.jpg");
    }

    return true;
}

DocumentWorker *Document::acquireWorker()
{
    m_workerReferences++;

    if (!m_worker) {
        m_worker = new DocumentWorker(this);
    }

    return m_worker;
}

void Document::releaseWorker()
{
    m_workerReferences--;
    if (m_workerReferences < 0) {
        qWarning() << "Unbalanced calls to acquireWorker/releaseWorker, refs:" << m_workerReferences;
        m_workerReferences = 0;
    }

    if (!m_worker) {
        qWarning() << "Tried to release dead worker";
        return;
    }

    if (m_workerReferences == 0) {
        m_worker->stop();
        m_worker = nullptr;
    }
}

void Document::setCurrentPage(int newPage)
{
    if (m_currentPage == newPage || newPage < 0) {
        return;
    }

    if (newPage >= m_pageCount) {
        return;
    }

    m_currentPage = newPage;

    emit currentPageChanged(newPage);
    emit templateChanged();
}
void Document::setPageCount(int pageCount)
{
    qDebug() << "Setting page count to" << pageCount;
    if (pageCount == m_pageCount) {
        return;
    }

    m_pageCount = pageCount;
    emit pageCountChanged();

    if (m_currentPage >= m_pageCount) {
        setCurrentPage(m_pageCount - 1);
    }
    storeMetadata();
}

void Document::storeMetadata()
{
    QFile metadataFile(getMetadataPath());
    if (metadataFile.open(QIODevice::WriteOnly)) {
        metadataFile.write(QByteArray::number(m_currentPage) + "\n");
        metadataFile.write(QByteArray::number(m_pageCount) + "\n");
        metadataFile.write(QByteArray::number(m_openCount) + "\n");
    }
}

void Document::storeTemplates()
{
    if (m_path.endsWith(".pdf")) {
        return;
    }

    QFile templateFile(m_path + ".pagedata");
    if (templateFile.open(QIODevice::WriteOnly)) {
        for (const QString pageTemplate : m_templates) {
            templateFile.write(pageTemplate.toUtf8() + '\n');
        }
    }
}

void Document::deletePages(QList<int> pagesToRemove)
{
    if (m_pageCount < pagesToRemove.count()) {
        return;
    }

    qDebug() << "Pruning pages" << pagesToRemove;
    QVector<QString> prunedTemplates;
    for(int i=0; i<m_templates.count(); i++) {
        if (!pagesToRemove.contains(i)) {
            prunedTemplates.append(m_templates[i]);
        }
    }
    m_templates = prunedTemplates;
    qDebug() << "Setting page count";
    setPageCount(m_pageCount - pagesToRemove.count());
    qDebug() << "Pages nuked";
    emit pagesDeleted(pagesToRemove);
}

void Document::addPage()
{
    m_templates.append(currentTemplate());
    setPageCount(m_pageCount + 1);
    setCurrentPage(m_pageCount - 1);
}

QString Document::currentTemplate()
{
    return templateForPage(m_currentPage);
}

QString Document::templateForPage(int page)
{
    if (m_path.endsWith(".pdf")) {
        return "Document";
    }

   if (page < 0 || page > m_templates.count()) {
       qWarning() << "Asked for template for invalid page" << page;
       return QString("Blank");
   }
   QString pageTemplate = m_templates.value(page);
   if (pageTemplate.isEmpty()) {
       pageTemplate = "Blank";
   }
   return pageTemplate;
}

void Document::setCurrentTemplate(QString newTemplate)
{
    if (newTemplate == currentTemplate() || m_currentPage > m_templates.count()) {
        return;
    }

    m_templates[m_currentPage] = newTemplate;
    emit templateChanged();
}

QStringList Document::availableTemplates() const
{
    if (m_path.endsWith(".pdf")) { // PDFs have no templates
        qDebug() << "This document is a pdf, no templates";
        return QStringList();
    } else {
        QDir templatesDir(Collection::collectionPath() + "/templates/");
        qDebug() << templatesDir.canonicalPath();
        QFileInfoList templatesInfo = templatesDir.entryInfoList(QStringList() << "*.png",
                                                   QDir::Files | QDir::Readable
                                                   );
        QStringList templates;
        templates << "Blank";
        for (const QFileInfo &templateInfo : templatesInfo) {
            qDebug() << templateInfo.absoluteFilePath();
            templates.append(templateInfo.baseName());
        }
        qDebug() << "We have these templates:" << templates;
        return templates;
    }
}

QString Document::getThumbnail(int index)
{
    QString thumbnailPath = getThumbnailPath(index);
    if (!QFile::exists(thumbnailPath)) {
        emit missingThumbnailRequested(index);
        return QStringLiteral("");
    }

    return "file://" + thumbnailPath;
}

QString Document::title()
{
    const QFileInfo pathInfo(m_path);

    if (pathInfo.isDir()) {
        return QDir(m_path).dirName();
    } else {
        return pathInfo.completeBaseName();
    }
}
