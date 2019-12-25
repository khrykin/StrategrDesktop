#include "notifierimplementation.h"
#include "utils.h"

#include <QDebug>
#include <QTimer>
#include "applicationsettings.h"


NotifierImplementation::NotifierImplementation(Strategy *strategy, QObject *parent)
        : strategy(strategy), QObject(parent) {
    setupTrayIcon();

    backend = new NotifierBackend(trayIcon, this);

    timer = new QTimer(this);
    connect(timer,
            &QTimer::timeout,
            this,
            &NotifierImplementation::timerTick);

    timer->start(ApplicationSettings::notifierTimerTimeInterval);

    timerTick();
}

void NotifierImplementation::setupTrayIcon() {
    contextMenu = new QMenu();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(contextMenu);
    trayIcon->show();
}

NotifierImplementation::~NotifierImplementation() {
    delete contextMenu;
    delete trayIcon;
    timer->stop();
}

void NotifierImplementation::timerTick() {
    const auto newUpcomingSession = strategy->upcomingSession();
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

    if (newUpcomingSession && *newUpcomingSession != upcomingSession) {
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

Session::Time NotifierImplementation::getCountdown() const {
    const auto countdown = nextIsTheEndOfStrategy
                           ? strategy->endTime() * 60 - currentSeconds()
                           : upcomingSession.beginTime() * 60 - currentSeconds();
    return countdown;
}

void NotifierImplementation::sendStartMessage(const Message &message) {
    backend->sendMessage(message.title, message.text);
    startSent = true;
    getReadySent = true;
}


void NotifierImplementation::sendPrepareMessage(const Message &message) {
    backend->sendMessage(message.title, message.text);
    getReadySent = true;
}

void NotifierImplementation::resetSents() {
    getReadySent = false;
    startSent = false;
}

void NotifierImplementation::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
}

QString NotifierImplementation::titleForSession(const Session &activitySession) {
    return QString::fromStdString(activitySession.activity->name())
           + " ("
           + humanTimeForMinutes(activitySession.duration())
           + ")";
}

std::tuple<NotifierImplementation::Message, NotifierImplementation::Message> NotifierImplementation::makeMessages() {
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

