#include "document.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QDir>

#define DEBUG_THIS
#include "debug.h"

Document::Document(QString path, QObject *parent)
    : QObject(parent),
      m_path(path),
      m_currentPage(0),
      m_pageCount(0),
      m_openCount(0)
{
    DEBUG_BLOCK;

    QFile metadataFile(path + ".metadata");
    if (metadataFile.open(QIODevice::ReadOnly)) {
        m_currentPage = metadataFile.readLine().trimmed().toInt();
        m_pageCount = metadataFile.readLine().trimmed().toInt();
        m_openCount = metadataFile.readLine().trimmed().toInt();
    }

    if (path.endsWith(".pdf")) {
        return;
    }

    m_templates.fill("Sketch", pageCount());

    QFile templatesFile(path + ".pagedata");
    if (!templatesFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to open" << templatesFile.fileName();
        return;
    }

    int page = 0;
    while (!templatesFile.atEnd()) {
        QString pageTemplate = templatesFile.readLine().trimmed();
        if (pageTemplate.isEmpty()) {
            break;
        }
        m_templates[page] = pageTemplate;
        page++;
    }

    connect(this, SIGNAL(currentPageChanged(int)), this, SLOT(storeMetadata()));
    connect(this, SIGNAL(openCountChanged()), this, SLOT(storeMetadata()));
    connect(this, SIGNAL(pageCountChanged()), this, SLOT(storeMetadata()));
    connect(this, SIGNAL(pageCountChanged()), this, SLOT(storeTemplates()));
    connect(this, SIGNAL(templateChanged()), this, SLOT(storeTemplates()));
}

Document::~Document()
{
    DEBUG_BLOCK;
}

void Document::addOpenCount()
{
    m_openCount++;
}

bool Document::createDocument(QString defaultTemplate, QString path)
{
    if (defaultTemplate.isEmpty()) {
        defaultTemplate = "Sketch";
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

    QFile templatesFile(path + ".pagedata");
    if (templatesFile.open(QIODevice::WriteOnly)) {
        templatesFile.write(defaultTemplate.toUtf8() + '\n');
        templatesFile.close();
    }

    return true;
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

    emit templateChanged();
    emit currentPageChanged(newPage);
}
void Document::setPageCount(int pageCount)
{
    if (pageCount == m_pageCount) {
        return;
    }

    m_pageCount = pageCount;
    emit pageCountChanged();

    if (m_currentPage >= m_pageCount) {
        setCurrentPage(m_pageCount - 1);
    }
}

void Document::storeMetadata()
{
    QFile metadataFile(m_path + ".metadata");
    if (metadataFile.open(QIODevice::WriteOnly)) {
        metadataFile.write(QByteArray::number(m_currentPage) + "\n");
        metadataFile.write(QByteArray::number(m_pageCount) + "\n");
        metadataFile.write(QByteArray::number(m_openCount) + "\n");
    }
}

void Document::storeTemplates()
{
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

    setPageCount(m_pageCount - pagesToRemove.count());
}

void Document::addPage()
{
    m_templates.append(currentTemplate());
    setPageCount(m_pageCount + 1);
    setCurrentPage(m_pageCount - 1);
}

QString Document::currentTemplate()
{
    if (m_path.endsWith(".pdf")) {
        return "Document";
    }
    if  (m_currentPage > m_templates.count()) {
        return "Sketch";
    }

    return m_templates.value(m_currentPage);
}

void Document::setCurrentTemplate(QString newTemplate)
{
    if (newTemplate == currentTemplate() || m_currentPage > m_templates.count()) {
        return;
    }

    m_templates[m_currentPage] = newTemplate;
    emit templateChanged();
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
    return QFileInfo(m_path).completeBaseName();
}
