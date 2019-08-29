//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

namespace ApplicationSettings {
    const auto windowMinimumWidth = 300;
    const auto windowMinimumHeight = 200;
    const auto windowInitialWidth = 400;

    const auto defaultSlotHeight = 35;
    const auto defaultActivityItemHeight = 40;
    const auto defaultPadding = 8;

    const auto currentTimeTimerInterval = 1 * 1000;
    const auto notifierTimerTimeInterval = 15 * 1000;
    const auto currentSessionShowDelay = 500;

    const auto rowBorderColor = "#ccc";

    const auto currentSessionHeight = 38;
    const auto overviewHeight = 8;

    const auto currentTimeMarkerColor = "#FF725C";

    extern const char *const version;
    extern const char *const shortVersion;
    extern const char *const repoName;
}


#endif //APPLICATIONSETTINGS_H
