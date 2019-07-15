#include "notifier.h"
#include "utils.h"

#include <QDebug>
#include <QTimer>
#include "applicationsettings.h"


Notifier::Notifier(Strategy *strategy, QObject *parent)
        : QObject(parent), strategy(strategy) {

    setupTrayIcon();

    backend = new NotifierBackend(trayIcon, this);

    timer = new QTimer(this);
    connect(timer,
            &QTimer::timeout,
            this,
            &Notifier::timerTick);

    timer->start(ApplicationSettings::notifierTimerTimeInterval);

    timerTick();
}

void Notifier::setupTrayIcon() {
    contextMenu = new QMenu();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(contextMenu);
    trayIcon->show();
}

Notifier::~Notifier() {
    delete contextMenu;
    delete trayIcon;
}

void Notifier::timerTick() {
    const auto newUpcomingSession =
            CurrentActivitySession::upcomingForStrategy(*strategy);
    auto strategyEndCountdown =
            strategy->endTime() * 60 - currentSeconds();

    if (!newUpcomingSession) {
        if (strategyEndCountdown <= getReadyInterval) {
            if (!nextIsTheEndOfStrategy) {
                resetSents();

                nextIsTheEndOfStrategy = true;
            }
        } else {
            nextIsTheEndOfStrategy = false;
            return;
        }
    }

    if (newUpcomingSession && newUpcomingSession != upcomingSession) {
        upcomingSession = *newUpcomingSession;
        resetSents();
    }

    auto countdown = getCountdown();
    auto[prepareMessage, startMessage] = makeMessages();

    if (countdown < startSentInterval && !startSent) {
        sendStartMessage(startMessage);
        return;
    }

    if (countdown < getReadyInterval && !getReadySent) {
        sendPrepareMessage(prepareMessage);
        return;
    }
}

ActivitySession::Time Notifier::getCountdown() const {
    const auto countdown = nextIsTheEndOfStrategy
                           ? strategy->endTime() * 60 - currentSeconds()
                           : upcomingSession.beginTime() * 60 - currentSeconds();
    return countdown;
}

void Notifier::sendStartMessage(const Message &message) {
    backend->sendMessage(message.title, message.text);
    startSent = true;
    getReadySent = true;
}


void Notifier::sendPrepareMessage(const Message &message) {
    backend->sendMessage(message.title, message.text);
    getReadySent = true;
}

void Notifier::resetSents() {
    getReadySent = false;
    startSent = false;
}

void Notifier::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
}

QString Notifier::titleForSession(const ActivitySession &activitySession) {
    return QString::fromStdString(activitySession.activity->name())
           + " ("
           + humanTimeForMinutes(activitySession.duration())
           + ")";
}

std::tuple<Notifier::Message, Notifier::Message> Notifier::makeMessages() {
    Message prepareMessage;
    Message startMessage;

    if (nextIsTheEndOfStrategy) {
        prepareMessage.title = "End Of A Strategy";
        startMessage.text = "Strategy ends right now";
    } else {
        prepareMessage.title = titleForSession(upcomingSession);
        startMessage.text = "Starts right now";
    }

    prepareMessage.text =
            QString("Coming up in %1 minutes")
                    .arg(Settings::getReadyMinutes);

    startMessage.title = prepareMessage.title;

    return std::make_tuple(prepareMessage, startMessage);
}

