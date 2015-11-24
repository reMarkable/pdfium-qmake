#ifndef EPNODE_H
#define EPNODE_H

#include <QtGui/QImage>
#include <QtCore/QRect>
#include <QDebug>
#include <memory>

struct EPNode
{
    virtual ~EPNode() {
        qDebug() << "epaper node dying";
    }

    struct EPNodeContent {
        virtual void draw(QPainter *) const = 0;

        QRect rect;
        QRect transformedRect;
        bool dirty;

        bool visible;

        int z;

        QTransform transform;
    };

    std::shared_ptr<EPNodeContent> content;
};

#endif // EPNODE_H
