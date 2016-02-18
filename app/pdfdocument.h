#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include "document.h"

#include <fpdfview.h>
#include <QMutex>

class PdfDocument : public Document
{
    Q_OBJECT
    Q_PROPERTY(QString currentTemplate READ currentTemplate WRITE setTemplate NOTIFY templateChanged)

public:
    PdfDocument(QString path, QObject *parent = nullptr);
    virtual ~PdfDocument();

signals:
    void templateChanged();

protected slots:
    virtual QImage loadOriginalPage(int index, QSize dimensions) override;
    void setTemplate(QString backgroundTemplate) override { Q_UNUSED(backgroundTemplate); }
    QStringList availableTemplates() const override { return QStringList("Document"); }
    QString currentTemplate() const override { return QStringLiteral("Document"); }

private:
    FPDF_DOCUMENT m_pdfDocument;
    QMutex m_destructionLock;
    bool m_initialized;
};

#endif // PDFDOCUMENT_H
