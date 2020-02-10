//
// Created by Dmitry Khrykin on 2019-07-13.
//

#include <iostream>

#include "notifierbackend.h"
#include "utility.h"
#include "applicationsettings.h"

#include <QSystemTrayIcon>
#include <QCoreApplication>

#ifdef Q_OS_WIN

#include "third-party/wintoast/wintoastlib.h"

class NotifierBackend::WintoastHandler : public WinToastLib::IWinToastHandler {
public:
    void toastActivated() const override {}

    void toastActivated(int) const override {}

    void toastDismissed(WinToastDismissalReason state) const override {}

    void toastFailed() const override {}
};

#endif

NotifierBackend::NotifierBackend() {
#ifdef Q_OS_WIN
    using namespace WinToastLib;

    if (!WinToast::isCompatible()) {
        std::cout << "Error, wintoast is not supported" << std::endl;
        return;
    }

    const auto appName = QCoreApplication::applicationName().toStdWString();
    const auto aumi = WinToast::configureAUMI(L"khrykin", appName, L"desktop",
                                              stg::text::wstring_from_utf8_string(ApplicationSettings::shortVersion));
    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(aumi);

    if (!WinToast::instance()->initialize()) {
        std::cout << "Error, could not initialize wintoast lib" << std::endl;
        return;
    }

    handler = new NotifierBackend::WintoastHandler();

    winToastInitialised = true;
#endif
}

#ifndef Q_OS_MAC
void NotifierBackend::sendMessage(const QString &title, const QString &message) {
#ifdef Q_OS_WIN
    if (!winToastInitialised) {
        fallbackSendMessage(title, message);
        return;
    }

    using namespace WinToastLib;
    WinToastTemplate templ = WinToastTemplate(WinToastTemplate::Text02);
    templ.setTextField(title.toStdWString(), WinToastTemplate::FirstLine);
    templ.setTextField(message.toStdWString(), WinToastTemplate::SecondLine);
    templ.setExpiration(60 * 1000);

    WinToast::WinToastError error;
    const bool launched = WinToast::instance()->showToast(templ, handler, &error);
    if (!launched) {
        std::cout << "Error: Could not launch toast notification. Error: "
                  << error << std::endl;
    }
#else
    fallbackSendMessage(title, message);
#endif
}
#endif

void NotifierBackend::fallbackSendMessage(const QString &title, const QString &message) {
    if (QSystemTrayIcon::supportsMessages()) {
        auto icon = QSystemTrayIcon();
        icon.showMessage(title, message, QIcon(), 10000);
    } else {
        std::cout << "Error: Can't send notification\n";
    }
}
