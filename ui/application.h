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
#include "selfupdater.h"

#ifdef Q_OS_MAC
Q_FORWARD_DECLARE_OBJC_CLASS(CocoaDelegate);
#endif

class Application : public QApplication {
public:
    Application(int &argc, char **argv);

#ifdef  Q_OS_MAC

    ~Application() override {
        releaseCocoaDelegate();
    }

#endif

    static QStringList openedFiles;
    static AboutWindow *aboutWindow;
    static UpdateDialog *updateDialog;

    static const SelfUpdater &updater();

    static void registerOpenedFile(const QString &filePath);
    static void clearRecentFiles();
    static void markFileClosed(const QString &filePath);
    static bool fileIsOpened(const QString &filePath);

#ifdef Q_OS_WIN
    static bool isGreaterThanWin8() {
        return _isGreaterThanWin8;
    }
#endif
private:
    static SelfUpdater _updater;

    static void setupFonts();

    bool launchedByOpenEvent = false;

#ifdef Q_OS_MAC

    static CocoaDelegate *cocoaDelegate;
    static void registerCocoaRecentFile(const QString &filePath);
    static void clearCocoaRecentFiles();

    void setupCocoaDelegate();
    void releaseCocoaDelegate();

#endif

#ifdef Q_OS_WIN
    static bool _isGreaterThanWin8;
#endif

    bool event(QEvent *event) override;
};

#endif //STRATEGR_APPLICATION_H
