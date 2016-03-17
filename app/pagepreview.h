#ifndef PAGEPREVIEW_H
#define PAGEPREVIEW_H

#include "documentworker.h"

#include <QQuickPaintedItem>
#include <QPointer>

class DocumentWorker;

class PagePreview : public QQuickPaintedItem
{
    Q_OBJECT

   Q_PROPERTY(Document *document WRITE setDocument READ getDocument NOTIFY documentChanged)

public:
    PagePreview();
    virtual ~PagePreview();

    virtual void paint(QPainter *painter) override;

    void setDocument(Document *document);
    Document *getDocument() { return m_document.data(); }

signals:
    void documentChanged();

private:
    QPointer<DocumentWorker> m_worker;
    QPointer<Document> m_document;
};

#endif // PAGEPREVIEW_H
