#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include "document.h"

#include <QStringList>

class ImageDocument : public Document
{
    Q_OBJECT

public:
    ImageDocument(QString path, QObject *parent = nullptr);

protected slots:
    QImage loadOriginalPage(int index, QSize dimensions);

private:
    QStringList m_files;
};

#endif // IMAGEDOCUMENT_H
