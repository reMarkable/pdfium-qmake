#include "document.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>
#include <QFile>
#include <QDataStream>

#define DEBUG_THIS
#include "debug.h"

Document::Document(QString path, QObject *parent)
    : QObject(parent),
      m_path(path),
      m_currentPage(0),
      m_pageCount(1)
{
    DEBUG_BLOCK;

    QFile metadataFile(path + ".metadata");
    if (metadataFile.open(QIODevice::ReadOnly)) {
        m_currentPage = metadataFile.readLine().trimmed().toInt();
        m_pageCount = metadataFile.readLine().trimmed().toInt();
    }

    if (path.endsWith(".pdf")) {
        return;
    }

    m_templates.fill(m_defaultTemplate, pageCount());

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
}

Document::~Document()
{
    DEBUG_BLOCK;

    QFile metadataFile(m_path + ".metadata");
    if (metadataFile.open(QIODevice::WriteOnly)) {
        metadataFile.write(QByteArray::number(m_currentPage) + "\n");
        metadataFile.write(QByteArray::number(m_pageCount) + "\n");
    }
}

void Document::setCurrentPage(int newPage)
{
    qDebug() << "changing page";
    if (m_currentPage == newPage || newPage < 0) {
        return;
    }

    if (newPage >= m_pageCount) {
        return;
    }

    m_currentPage = newPage;

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

void Document::deletePages(QList<int> pagesToRemove)
{
    if (m_pageCount < pagesToRemove.count()) {
        return;
    }

    setPageCount(m_pageCount - pagesToRemove.count());
}

QString Document::currentTemplate()
{
    if (m_path.endsWith(".pdf")) {
        return "Document";
    }

    return m_templates.value(m_currentPage);
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
