#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_THIS

#include <QDebug>
#include <QElapsedTimer>

static int debugHelper_indent = 0;
struct DebugHelperClass {
    DebugHelperClass(QString funcInfo) : m_funcInfo(funcInfo) {
        //qDebug().noquote() << QByteArray(debugHelper_indent, '\t') << '>' << m_funcInfo;
        m_timer.start();
        debugHelper_indent++;
    }

    ~DebugHelperClass() {
        debugHelper_indent--;
        if (m_timer.elapsed() > 10) {
            qDebug().noquote() << QByteArray(debugHelper_indent, '\t') << '<' << m_funcInfo << m_timer.elapsed() << "ms";
        }
    }

    QString m_funcInfo;
    QElapsedTimer m_timer;
};

#define DEBUG_BLOCK DebugHelperClass debugHelper__Block(__PRETTY_FUNCTION__);

#else

#define DEBUG_BLOCK

#endif // DEBUG_THIS

#endif // DEBUG_H
