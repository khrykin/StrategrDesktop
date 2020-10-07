//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_MACOSWINDOW_H
#define STRATEGR_MACOSWINDOW_H

#include <QPixmap>
#include <QtCore>

#ifdef Q_OS_MAC

class MainWindow;
class MacOSWindow {
public:
    static void setup(MainWindow *window);
    static double toolbarHeight(MainWindow *window);

    static void pageDidChanged(MainWindow *window, int pageIndex);
    static void updateWindowTitle(MainWindow *window);

    static QRect adjustedGeometry(MainWindow *window);

    static QPixmap resizeCursor();
    static QPixmap closedHandCursor();
    static QPixmap openHandCursor();

    static bool hasSFSymbol();

private:
    static NSString *makeToolbarIdentifier(const MainWindow *window);
};

#endif


#endif//STRATEGR_MACOSWINDOW_H
