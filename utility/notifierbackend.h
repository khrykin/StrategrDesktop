#ifndef NOTIFIERBACKEND_H
#define NOTIFIERBACKEND_H

#include <QString>

class NotifierBackend {
public:
    NotifierBackend();
    void sendMessage(const QString& title, const QString& message);

private:
    static void fallbackSendMessage(const QString &title, const QString &message);

#ifdef Q_OS_WIN
    class WintoastHandler;
    WintoastHandler *handler = nullptr;
    bool winToastInitialised = false;
#endif
};


#endif // NOTIFIERBACKEND_H
