#include "nativedocument.h"

#include "collection.h" // To get the collection path
#include <QMap>
#include <QDir>
#include <QFile>
#include <QDebug>

#include <algorithm>

#if 0
NativeDocument::NativeDocument(QString documentPath, QString defaultTemplate, QObject *parent) :
    Document(documentPath, parent),
    m_defaultTemplate(defaultTemplate)
{
}

NativeDocument::~NativeDocument()
{
    QFile metadataFile(path() + ".pagedata");
    if (!metadataFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Unable to open" << metadataFile.fileName();
        return;
    }

    for(const QString &pageTemplate : m_templates) {
        metadataFile.write(pageTemplate.toUtf8() + "\n");
    }
}

void NativeDocument::setTemplate(QString backgroundTemplate)
{
    if (!s_templateLoader.templates.contains(backgroundTemplate)) {
        qWarning() << Q_FUNC_INFO << "Unknown template" << backgroundTemplate;
        return;
    }

    m_templates[currentIndex()] = backgroundTemplate;
    setCurrentBackground(s_templateLoader.templates[backgroundTemplate]);
    m_defaultTemplate = backgroundTemplate;

    emit templateChanged();
}

QStringList NativeDocument::availableTemplates() const
{
    return s_templateLoader.templates.keys();
}

QString NativeDocument::currentTemplate() const
{
    return m_templates.value(currentIndex());
}

void NativeDocument::addPage(QString backgroundTemplate)
{
    if (backgroundTemplate.isEmpty()) {
        backgroundTemplate = m_defaultTemplate;
    }

    m_templates.append(backgroundTemplate);
    setPageCount(pageCount() + 1);
    setCurrentIndex(pageCount() - 1);
    setCurrentBackground(s_templateLoader.templates[backgroundTemplate]);
}

void NativeDocument::deletePages(QList<int> pagesToRemove)
{
    std::sort(pagesToRemove.begin(), pagesToRemove.end());

    QVector<QString> newTemplates;
    for (int i=0; i<m_templates.count(); i++) {
        if (pagesToRemove.contains(i)) {
            continue;
        }
        newTemplates.append(m_templates[i]);
    }

    m_templates = newTemplates;

    Document::deletePages(pagesToRemove);
}

QImage NativeDocument::loadOriginalPage(int index, QSize dimensions)
{
    if (index < 0 || index > m_templates.count()) {
        return QImage();
    }

    QString templateName = m_templates.value(index);

    if (templateName.isEmpty()) {
        templateName = m_defaultTemplate;
    }

    if (templateName.isEmpty() || !s_templateLoader.templates.contains(templateName)) {
        if (s_templateLoader.templates.isEmpty()) {
            qWarning() << "no templates available!";
            return QImage();
        }
        qWarning() << Q_FUNC_INFO << "asked for invalid template:" << templateName;
        return s_templateLoader.templates.first();
    }

    return s_templateLoader.templates.value(templateName).scaled(dimensions);
}

#endif
