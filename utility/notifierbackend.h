#ifndef NOTIFIERBACKEND_H
#define NOTIFIERBACKEND_H

#include <QObject>
#include <QString>
#include <QSystemTrayIcon>

class NotifierBackend : QObject {
Q_OBJECT
public:
    explicit NotifierBackend(QSystemTrayIcon *trayIcon,
                             QObject *parent = nullptr);

    void sendMessage(const QString &title, const QString &message);
private:
    QSystemTrayIcon *trayIcon;
};


#endif // NOTIFIERBACKEND_H
