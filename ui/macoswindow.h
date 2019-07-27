//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_MACOSWINDOW_H
#define STRATEGR_MACOSWINDOW_H

#include <QtCore>
#include <QPixmap>

#ifdef Q_OS_MAC

class MainWindow;
class MacOSWindow {
public:
    static void setup(MainWindow *window);
    static void teardown(MainWindow *window);

    static void pageChange(MainWindow *window, int pageIndex);
    static void updateWindowTitle(MainWindow *window);

    static QRect geometry(MainWindow *window);

    static QPixmap resizeCursor();
    static QPixmap closedHandCursor();
    static QPixmap openHandCursor();
    static QPixmap dragCopyCursor();
private:
};

#endif


#endif //STRATEGR_MACOSWINDOW_H
