#include "imagedocument.h"

#include <QDebug>
#include <QDir>
#include <QStringList>

ImageDocument::ImageDocument(QString path, QObject *parent) :
    Document(path, parent)
{
    QDir dir(path);
    const QStringList imageFormats({"*.png", "*.jpg"});

    QFileInfoList files = dir.entryInfoList(imageFormats);

    foreach (const QFileInfo &file, files) {
        m_files.append(file.absoluteFilePath());;
    }

    setPageCount(m_files.count());
}

void ImageDocument::loadBackground(int index)
{
    if (index >= m_files.size()) {
        return;
    }
    if (index < 0) {
        return;
    }

    if (cachedIndices().contains(index)) {
        return;
    }
    qDebug() << "loading" << index << "/" << m_files.size();

    QImage image(m_files[index]);
    image = image.scaled(dimensions(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    emit backgroundLoaded(image.convertToFormat(QImage::Format_RGB16), index);
}
