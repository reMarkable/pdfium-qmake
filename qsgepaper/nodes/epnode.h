#ifndef EPNODE_H
#define EPNODE_H

#include <QtGui/QImage>
#include <QtCore/QRect>
#include <QDebug>

class EPNode
{
public:
    EPNode();
    virtual ~EPNode() {
        qDebug() << "epaper node dying";
    }

    virtual void draw(QPainter *) const {}

    QRect rect;

    QRect transformedRect;

    quint64 id;

    bool dirty;

    bool visible;

    int z;

    QTransform transform;

    virtual bool fast() = 0;

    inline bool operator==(const EPNode &other) const {
        return other.id == id && other.transformedRect == transformedRect && other.transform == transform;
    }
    inline bool operator<(const EPNode &other) {
        return z < other.z;
    }
};


//inline uint qHash(const EpaperNode &rect, uint seed)
//{
//    return (qHash(rect.id, seed) ^ rect.transformedRect.x() ^ rect.transformedRect.y() ^ rect.transformedRect.width() ^ rect.transformedRect.height());
//}


#endif // EPNODE_H
