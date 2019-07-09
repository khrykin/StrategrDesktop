//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QDesktopWidget>

namespace ApplicationSettings {
    const auto windowMinimumWidth = 200;
    const auto windowMinimumHeight = 200;
    const auto windowInitialWidth = 400;

    const auto lastOpenedDirectoryKey = "lastOpenedDirectory";
    const auto lastOpenedStrategyKey = "lastOpenedStrategy";
    const auto recentFilesKey = "recentFiles";
    const auto defaultStrategyKey = "defaultStrategy";
    const auto defaultActivitiesKey = "defaultActivities";
    const auto numberOfRecent = 5;
}

#endif //APPLICATIONSETTINGS_H
