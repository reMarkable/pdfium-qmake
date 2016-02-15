#ifndef NATIVEDOCUMENT_H
#define NATIVEDOCUMENT_H

#include "document.h"

class NativeDocument : public Document
{
    Q_OBJECT

    Q_PROPERTY(QString currentTemplate READ currentTemplate WRITE setTemplate NOTIFY templateChanged)

public:
    NativeDocument(QString path, QString defaultTemplate = QStringLiteral("Blank"), QObject *parent = nullptr);
    ~NativeDocument();

public slots:
    void setTemplate(QString backgroundTemplate);
    static QStringList availableTemplates();
    QString currentTemplate();

signals:
    void templateChanged();

protected slots:
    QImage loadOriginalPage(int index, QSize dimensions);

private:
    QHash<int, QString> m_templates;
    QString m_defaultTemplate;
};

#endif // NATIVEDOCUMENT_H
