#include "pagepreview.h"
#include <QPainter>

PagePreview::PagePreview() :
    m_document(nullptr)
{
    qDebug() << "Page preview created";
}

PagePreview::~PagePreview()
{
    if (!m_document.isNull()) {
        qDebug() << "Releasing worker";
        m_document->releaseWorker();
    }
    qDebug() << "Page preview destroyed";
}

void PagePreview::paint(QPainter *painter)
{
     if (!m_worker) {
         painter->fillRect(contentsBoundingRect(), Qt::white);
         return;
     }

     if (m_worker->pageContents.isNull()) {
         painter->drawImage(contentsBoundingRect(), m_worker->background());
         return;
     }

     painter->drawImage(contentsBoundingRect(), m_worker->pageContents);
}

void PagePreview::setDocument(Document *document)
{
    if (document == m_document) {
        return;
    }

    if (m_worker) {
        disconnect(m_worker, SIGNAL(updateNeeded()), this, SLOT(update()));
        disconnect(m_worker, SIGNAL(redrawingNeeded()), this, SLOT(update()));
    }

    if (m_document) {
        m_document->releaseWorker();
    }

    m_document = document;
    m_worker = document->acquireWorker();

    if (m_worker) {
        connect(m_worker, SIGNAL(updateNeeded()), this, SLOT(update()));
        connect(m_worker, SIGNAL(redrawingNeeded()), this, SLOT(update()));
    }
}
