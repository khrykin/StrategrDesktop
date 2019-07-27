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

class Application : public QApplication {
public:
    Application(int &argc, char **argv);
    static QStringList openedFiles;

private:
    bool launchedByOpenEvent = false;

    MainWindow *initialWindow = nullptr;

    bool event(QEvent *event) override;

    static void setupFonts();
};

#endif //STRATEGR_APPLICATION_H
