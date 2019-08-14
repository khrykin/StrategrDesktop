//
// Created by Dmitry Khrykin on 2019-07-15.
//

#ifndef STRATEGR_APPLICATION_H
#define STRATEGR_APPLICATION_H

#include <QApplication>
#include <QFileOpenEvent>
#include <QtDebug>
#include <QTimer>

#include "mainwindow.h"

Q_FORWARD_DECLARE_OBJC_CLASS(CocoaDelegate);

class Application : public QApplication {
public:
    Application(int &argc, char **argv);

#ifdef  Q_OS_MAC

    ~Application() override {
        releaseCocoaDelegate();
    }

#endif

    static QStringList openedFiles;

private:
    static void setupFonts();

    bool launchedByOpenEvent = false;

#ifdef Q_OS_MAC
    static CocoaDelegate *cocoaDelegate;
    void setupCocoaDelegate();
    void releaseCocoaDelegate();
#endif

    bool event(QEvent *event) override;
};

#endif //STRATEGR_APPLICATION_H
