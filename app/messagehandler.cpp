/**
  * Static message handler that is initialized on application start
  * Probably bad form, hard to discover idk, fuck you.
  */

#include <QFile>
#include <QByteArray>
#include <iostream>

#include "messagehandler.h"

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);


static MessageHandler s_messageHandler;


void MessageHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString typeText;
    QString typeColor;
    switch (type) {
    case QtDebugMsg:
        typeColor = QStringLiteral("\033[02;32m");
        typeText = QStringLiteral("Debug");
        break;
    case QtInfoMsg:
        typeColor = QStringLiteral("\033[01;32m");
        typeText = QStringLiteral("Info");
        break;
    case QtWarningMsg:
        typeColor = QStringLiteral("\033[01;33m");
        typeText = QStringLiteral("Warning");
        break;
    case QtCriticalMsg:
        typeColor = QStringLiteral("\033[01;31m");
        typeText = QStringLiteral("Critical");
        break;
    case QtFatalMsg:
        typeColor = QStringLiteral("\033[01;31m");
        typeText = QStringLiteral("Fatal");
        break;
    }

    { // Print to log file
        QString text(QStringLiteral("%1: %2 (%3:%4, %5)").arg(typeText).arg(msg).arg(context.file).arg(context.line).arg(context.function));
        s_messageHandler.logFile.write(text.toUtf8() + '\n');
    }

    if (type == QtDebugMsg) {
        if (s_messageHandler.debugEnabled) {
            std::cout << msg.toStdString() << std::endl;
        }

        return;
    }

    { // Print to stdout
        QString text(QStringLiteral("%1%2\033[0m: %3 (%4:%5, %6)").arg(typeColor).arg(typeText).arg(msg).arg(context.file).arg(context.line).arg(context.function));
        std::cout << text.toStdString() << std::endl;
    }
}

MessageHandler::MessageHandler() :
    logFile("log.txt"),
    debugEnabled(true)
{
    if (!logFile.open(QIODevice::WriteOnly)) {
        return;
    }

    qInstallMessageHandler(messageHandler);
}

MessageHandler::~MessageHandler()
{
    if (logFile.isOpen()) {
        qInstallMessageHandler(0);
    }
}

void MessageHandler::enableDebugOutput(bool enabled)
{
    s_messageHandler.debugEnabled = enabled;
}
