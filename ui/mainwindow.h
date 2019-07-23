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
    static const bool LoadInCurrentWindow = false;

    explicit MainWindow(QWidget *parent = nullptr);

    static MainWindow *createLastOpened();

    ~MainWindow() override;

    MainScene *scene() const;
    ApplicationMenu *menu() const;

    void openNewWindow();
    void openFile();
    void openRecentFile();
    void loadFile(const QString &path, bool inNewWindow = true);

    void saveFile();
    void saveFileAs();

    void saveCurrentStrategyAsDefault();
    void clearRecentFilesList();


private:
    friend ApplicationMenu;

    explicit MainWindow(FileSystemIOManager fsIOManager,
                        QWidget *parent = nullptr);

    MainScene *_scene = nullptr;
    ApplicationMenu *_menu = nullptr;
    FileSystemIOManager fsIOManager = FileSystemIOManager(this);
    std::unique_ptr<Strategy> strategy = fsIOManager.openDefaultStrategy();

    void setIsSaved(bool isSaved);
    void setStrategy(std::unique_ptr<Strategy> newStrategy);
    void strategyStateChanged();
    void updateWindowTitle();

    bool wantToClose();
    void closeEvent(QCloseEvent *event) override;
    void setup();
    void tearDown();

};


#endif //UI_MAINWINDOW_H
