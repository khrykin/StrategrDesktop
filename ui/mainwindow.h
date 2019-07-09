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

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    Strategy openRecentOrCreateNew();

    MainScene *scene() const;
    ApplicationMenu *menu() const;

    void openNewWindow();
    void openFile();

    void saveFile();
    void saveFileAs();

    void saveCurrentStrategyAsDefault();
    void clearRecentFilesList();

    void openRecentFile();

    void loadFile(const QString &path);
private:
    friend ApplicationMenu;

    MainScene *_scene = nullptr;
    ApplicationMenu *_menu = nullptr;
    FileSystemIOManager fsIOManager = FileSystemIOManager(this);
    Strategy strategy = fsIOManager.openDefaultStrategy();

    void setIsSaved(bool isSaved);
    void setStrategy(const Strategy &newStrategy);
    void strategyStateChanged();
    void updateWindowTitle();

    bool wantToClose();
    void closeEvent(QCloseEvent *event) override;
};


#endif //UI_MAINWINDOW_H
