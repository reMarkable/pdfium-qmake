/**
  * Static message handler that is initialized on application start
  * Probably bad form, hard to discover idk, fuck you.
  */

#include <QFile>
#include <QByteArray>
#include <iostream>
#include <iomanip>
#include <QTime>

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

    if (type == QtDebugMsg && !s_messageHandler.debugEnabled) {
        return;
    }

    // Print to stdout
    {
        std::cout << QTime::currentTime().toString("mm:ss.zzz").toStdString() << " "
                  << typeColor.toStdString();

        // std::cout/std::string doesn't handle nullptrs well
        std::string fileName;
        if (context.file) {
            fileName = context.file;
        }
        QString functionName(context.function);

        // If it is a class,
        int classIndex = functionName.indexOf("::");
        if (classIndex != -1) {
            QString className = functionName.left(classIndex).split(' ').last().leftJustified(20);
            functionName = functionName.mid(classIndex + 2);
            std::cout << className.leftJustified(20).toStdString();
        } else {
            std::cout << QStringLiteral("").leftJustified(20).toStdString();
        }

        std::cout << msg.toStdString() << " "
                  << fileName << ":" << context.line << " "
                  << "(" << functionName.toStdString() << ")"
                  << "\033[0m"
                  << std::endl;
    }
}

MessageHandler::MessageHandler() :
    logFile("log.txt"),
    debugEnabled(true)
{
    if (!logFile.open(QIODevice::WriteOnly)) {
        std::cerr << "messagehandler: unable to open log.txt!" << std::endl;
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
