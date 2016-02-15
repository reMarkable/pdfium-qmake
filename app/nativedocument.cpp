#include "nativedocument.h"

#include "collection.h" // To get the collection path
#include <QMap>
#include <QDir>
#include <QFile>
#include <QDebug>

namespace { // Local to this file
struct TemplateLoader {
    TemplateLoader() {
        QDir dir(Collection::collectionPath() + "/templates/");
        QFileInfoList files = dir.entryInfoList(QStringList("*.png"));
        for (QFileInfo fileInfo : files) {
            templates.insert(fileInfo.baseName(), QImage(fileInfo.absoluteFilePath()));
        }
        qDebug() << templates.keys();
    }

    QMap<QString, QImage> templates;
};
}

static TemplateLoader s_templateLoader;

NativeDocument::NativeDocument(QString documentPath, QString defaultTemplate, QObject *parent) :
    Document(documentPath, parent),
    m_defaultTemplate(defaultTemplate)
{
    if (!s_templateLoader.templates.isEmpty() && !s_templateLoader.templates.contains(defaultTemplate)) {
        m_defaultTemplate = s_templateLoader.templates.keys().first();
    }

    if (pageCount() < 1) {
        setPageCount(1);
    }

    QFile metadataFile(path() + ".pagedata");
    if (!metadataFile.open(QIODevice::ReadOnly)) {
        return;
    }

    int page = 0;
    while (!metadataFile.atEnd()) {
        QString pageTemplate = metadataFile.readLine().trimmed();
        if (pageTemplate.isEmpty()) {
            break;
        }
        m_templates[page] = pageTemplate;
        page++;
    }
}

NativeDocument::~NativeDocument()
{
    QFile metadataFile(path() + ".pagedata");
    if (!metadataFile.open(QIODevice::WriteOnly)) {
        return;
    }

    for (int i=0; i<pageCount(); i++) {
        if (!m_templates.contains(i)) {
            metadataFile.write(m_defaultTemplate.toUtf8() + "\n");
        } else {
            metadataFile.write(m_templates.value(i).toUtf8() + "\n");
        }
    }
}

void NativeDocument::setTemplate(QString backgroundTemplate)
{
    if (!s_templateLoader.templates.contains(backgroundTemplate)) {
        return;
    }

    m_templates[currentIndex()] = backgroundTemplate;
    setCurrentBackground(s_templateLoader.templates[backgroundTemplate]);

    emit templateChanged();
}

QStringList NativeDocument::availableTemplates()
{
    return s_templateLoader.templates.keys();
}

QString NativeDocument::currentTemplate()
{
    if (!m_templates.contains(currentIndex())) {
        return m_defaultTemplate;
    }

    return m_templates.value(currentIndex());
}

QImage NativeDocument::loadOriginalPage(int index, QSize dimensions)
{
    if (index < 0 || index > pageCount()) {
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
        return s_templateLoader.templates.first();
    }

    return s_templateLoader.templates.value(templateName);
}
