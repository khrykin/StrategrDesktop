#ifndef NOTIFIERBACKEND_H
#define NOTIFIERBACKEND_H
#include <QObject>
#include <QString>
#include <QSystemTrayIcon>

class NotifierBackend : QObject {
  Q_OBJECT
  QSystemTrayIcon *trayIcon;

public:
  NotifierBackend(QSystemTrayIcon *trayIcon, QObject *parent = nullptr);
  void sendMessage(QString title, QString message);
};

#endif // NOTIFIERBACKEND_H
