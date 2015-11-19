#include "epnode.h"

EPNode::EPNode()
{
//    qDebug() << "epaper node spawning";
    static quint64 new_id = 0;
    id = ++new_id;

    dirty = false;
}

