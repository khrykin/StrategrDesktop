//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QCoreApplication>

namespace ApplicationSettings {
    const auto windowMinimumWidth = 300;
    const auto windowMinimumHeight = 200;
    const auto windowInitialWidth = 350;

    const auto sessionFontSize = 14;

    const auto defaultSlotHeight = 35;
    const auto defaultActivityItemHeight = 40;
    const auto defaultPadding = 8;

    const auto currentSessionHeight = 38;
    const auto overviewHeight = 8;

    const auto currentTimeTimerSecondsInterval = 1;
    const auto notifierTimerSecondsInterval = 15;
    const auto currentSessionShowDelay = 200;

    const auto rowBorderColor = "#ccc";
    const auto currentTimeMarkerColor = "#ff725c";

    const auto fontResourcePath = "FontAwesome";

#if defined(Q_OS_WIN32) && !defined(Q_OS_WIN64)
    const auto architectureString = "32";
#else
    const auto architectureString = "64";
#endif

    extern const char *const version;
    extern const char *const shortVersion;
    extern const char *const repoName;
    extern const char *const appcastURL;
}


#endif //APPLICATIONSETTINGS_H
