#ifndef IMAGEDOCUMENT_H
#define IMAGEDOCUMENT_H

#include "document.h"

#include <QStringList>

class ImageDocument : public Document
{
public:
    ImageDocument(QString path, QObject *parent = nullptr);

protected slots:
    void loadBackground(int index);

private:
    QStringList m_files;
};

#endif // IMAGEDOCUMENT_H
