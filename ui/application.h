//
// Created by Dmitry Khrykin on 2019-07-15.
//

#ifndef STRATEGR_APPLICATION_H
#define STRATEGR_APPLICATION_H

#include <QApplication>
#include <QFileOpenEvent>
#include <QtDebug>
#include <QTimer>

#include "aboutwindow.h"
#include "updatedialog.h"
#include "mainwindow.h"

class Application : public QApplication {
public:
    Application(int &argc, char **argv);

    static QStringList openedFiles;
    static AboutWindow *aboutWindow;
    static UpdateDialog *updateDialog;

    static const SelfUpdater &updater();
private:
    static SelfUpdater _updater;

    bool launchedByOpenEvent = false;

    MainWindow *initialWindow = nullptr;

    static void setupFonts();

    bool event(QEvent *event) override;
};

#endif //STRATEGR_APPLICATION_H
