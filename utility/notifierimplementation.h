#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <tuple>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include "strategy.h"
#include "notifierbackend.h"

class NotifierImplementation : public QObject {
Q_OBJECT
public:
    explicit NotifierImplementation(stg::strategy *strategy,
                                    QObject *parent = nullptr);
    ~NotifierImplementation() override;
    void setStrategy(stg::strategy *newStrategy);
    void timerTick();

signals:

public slots:

private:
    struct Message {
        QString title;
        QString text;
    };

    struct Settings {
        static const auto getReadyMinutes = 5;
        static const auto startSeconds = 20;
    };

    const static auto getReadyInterval = Settings::getReadyMinutes * 60;
    const static auto startSentInterval = Settings::startSeconds;

    stg::strategy *strategy;
    stg::session upcomingSession;

    QTimer *timer = nullptr;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *contextMenu = nullptr;
    NotifierBackend *backend = nullptr;

    bool getReadySent = false;
    bool startSent = false;
    bool nextIsTheEndOfStrategy = false;

    QString titleForSession(const stg::session &activitySession);
    void setupTrayIcon();

    void sendPrepareMessage(const Message &message);
    void sendStartMessage(const Message &message);

    std::tuple<Message, Message> makeMessages();

    void resetSents();
    stg::session::time_t getCountdown() const;
};

#endif // NOTIFIER_H
