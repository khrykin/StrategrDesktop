//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef WINDOWGEOMETRYMANAGER_H
#define WINDOWGEOMETRYMANAGER_H

#include "applicationsettings.h"

#include <QSettings>
#include <QWidget>

class MainWindow;
class WindowGeometryManager {
public:
    static void setInitialGeometry(MainWindow *window);
    static void saveGeometry(MainWindow *window);
    static void resetSavedGeometry();

private:
    static QRect defaultInitialRect(QWidget *window);
    static QSize avaliableDesktopSize(QWidget *widget);
    static QRect initRectForExtraWindows(const QRect &defaultRect, QWidget *window);

    static constexpr auto windowGeometrySetting = "windowGeometry";
    static constexpr auto defaultAdjustFactor = 30;

    static QWidgetList windows;
};

#endif //WINDOWGEOMETRYMANAGER_H
