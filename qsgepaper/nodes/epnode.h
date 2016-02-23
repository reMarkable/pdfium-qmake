#ifndef EPNODE_H
#define EPNODE_H

#include <QtGui/QImage>
#include <QtCore/QRect>
#include <QDebug>
#include <memory>

struct EPNode
{
    virtual ~EPNode();

    struct Content {
        virtual ~Content();
        virtual void draw(QPainter *) const = 0;

        QRect rect;
        QRect transformedRect;
        bool dirty;

        bool visible;

        int z;
        int order;

        QTransform transform;
    };

    std::shared_ptr<Content> content;
};

#endif // EPNODE_H
