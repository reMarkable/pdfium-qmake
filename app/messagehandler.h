#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QFile>
#include <QByteArray>
#include <iostream>


struct MessageHandler {
    MessageHandler();
    ~MessageHandler();

    static void enableDebugOutput(bool enabled);
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    QFile logFile;
    bool debugEnabled;
};

#endif // MESSAGEHANDLER_H
