//
// Created by Dmitry Khrykin on 2019-07-08.
//

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMenu>
#include <QCloseEvent>

#include "strategy.h"
#include "filesystemiomanager.h"
#include "windowgeometrymanager.h"
#include "mainscene.h"
#include "applicationmenu.h"
#include "macoswindow.h"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    static const bool loadInCurrentWindow = false;

    explicit MainWindow(QWidget *parent = nullptr);

    static MainWindow *createLastOpened();
    static void openNewWindow();

    ~MainWindow() override;

    MainScene *scene() const;
    ApplicationMenu *menu() const;

    void openFile();
    void openRecentFile();
    void loadFile(const QString &path, bool inNewWindow = true);
    void saveFile();
    void saveFileAs();

    void saveCurrentStrategyAsDefault();
    void clearRecentFilesList();

    void reloadStrategy();
private:
    friend ApplicationMenu;

#ifdef Q_OS_MAC
    friend MacOSWindow;
#endif

    explicit MainWindow(FileSystemIOManager existingFsIOManager,
                        QWidget *parent = nullptr);

    MainScene *_scene = nullptr;
    ApplicationMenu *_menu = nullptr;
    FileSystemIOManager fsIOManager = FileSystemIOManager(this);
    stg::strategy strategy;

    void setIsSaved(bool isSaved);
    void setStrategy(const stg::strategy &newStrategy);
    void strategyStateChanged();
    void updateWindowTitle();

    bool wantToClose();

    void setup();
    void teardown();

protected:
    void closeEvent(QCloseEvent *event) override;
};


#endif //UI_MAINWINDOW_H
