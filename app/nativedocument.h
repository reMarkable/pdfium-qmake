#ifndef NATIVEDOCUMENT_H
#define NATIVEDOCUMENT_H

#include "document.h"

class NativeDocument : public Document
{
    Q_OBJECT

    Q_PROPERTY(QString currentTemplate READ currentTemplate WRITE setTemplate NOTIFY templateChanged)

public:
    NativeDocument(QString path, QString defaultTemplate = QString(), QObject *parent = nullptr);
    ~NativeDocument();

public slots:
    void setTemplate(QString backgroundTemplate) override;
    QStringList availableTemplates() const override;
    QString currentTemplate() const override;
    void addPage();
    void deletePages(QList<int> pagesToRemove) override;

signals:
    void templateChanged();

protected slots:
    QImage loadOriginalPage(int index, QSize dimensions) override;

private:
    QVector<QString> m_templates;
    QString m_defaultTemplate;
};

#endif // NATIVEDOCUMENT_H
