//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QAction>

#include "applicationmenu.h"
#include "mainwindow.h"

ApplicationMenu::ApplicationMenu(MainWindow *window) : window(window) {
    setupFileMenu();
    setupEditMenu();
    setupViewMenu();

    window->setMenuBar(this);
}

void ApplicationMenu::setupEditMenu() {
    addMenu(editMenu);

    // TODO
//
//    editMenu->addAction(slotBoard->groupsList()->getRedoAction());
//
//    editMenu->addSeparator();
//
//    editMenu->addAction(slotBoard->groupsList()->getSelectAllAction());
//
//    editMenu->addAction(slotBoard->groupsList()->getClearSelectionAction());
}

void ApplicationMenu::setupViewMenu() {
    addMenu(viewMenu);

    viewMenu->addSeparator();

    viewMenu->addAction("Edit Activities",
                        window->scene(),
                        &MainScene::showActivities,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));

    viewMenu->addSeparator();

    viewMenu->addAction("Go to Current Time",
                        window->scene(),
                        &MainScene::focusOnCurrentTime,
                        QKeySequence(Qt::CTRL + Qt::Key_Slash));

    viewMenu->addSeparator();
}

void ApplicationMenu::setupFileMenu() {
    addMenu(fileMenu);

    fileMenu->addAction("New",
                        window,
                        &MainWindow::openNewWindow,
                        QKeySequence(Qt::CTRL + Qt::Key_N));

    fileMenu->addAction("Open",
                        window,
                        &MainWindow::openFile,
                        QKeySequence(Qt::CTRL + Qt::Key_O));

    setupRecentMenu();

    fileMenu->addSeparator();

    fileMenu->addAction("Save",
                        window,
                        &MainWindow::saveFile,
                        QKeySequence(Qt::CTRL + Qt::Key_S));

    fileMenu->addAction("Save As",
                        window,
                        &MainWindow::saveFileAs,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));

    fileMenu->addSeparator();

    fileMenu->addAction("Close",
                        window,
                        &MainWindow::close,
                        QKeySequence(Qt::CTRL + Qt::Key_W));

    fileMenu->addSeparator();

    fileMenu->addAction("Settings",
                        window->scene(),
                        &MainScene::openStrategySettings,
                        QKeySequence(Qt::CTRL + Qt::Key_Comma));

    fileMenu->addAction("Save Current Strategy as Default",
                        window,
                        &MainWindow::saveCurrentStrategyAsDefault,
                        QKeySequence(Qt::CTRL + Qt::Key_D));
}

void ApplicationMenu::setupRecentMenu() {
    fileMenu->addMenu(recentMenu);

    createRecentFilesActions();

    for (int i = 0; i < FileSystemIOManager::Settings::numberOfRecent; ++i) {
        recentMenu->addAction(recentFileActions[i]);
    }

    recentMenu->addSeparator();

    recentMenu->addAction("Clear List",
                          window,
                          &MainWindow::clearRecentFilesList);

    updateRecentFilesActions();
}

void ApplicationMenu::updateRecentFilesActions() {
    auto recentPaths = FileSystemIOManager::recentPaths();
    auto numRecentFiles = qMin(recentPaths.size(),
                               FileSystemIOManager::Settings::numberOfRecent);
    auto recentFileNames = FileSystemIOManager::recentFileNames();

    if (numRecentFiles != 0) {
        for (auto &action : recentMenu->actions()) {
            action->setVisible(true);
        }
    }

    for (auto i = 0; i < numRecentFiles; ++i) {
        QString text = recentFileNames[i];
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(recentPaths[i]);
        recentFileActions[i]->setVisible(true);
    }

    // Hide all extra actions
    for (auto j = numRecentFiles;
         j < FileSystemIOManager::Settings::numberOfRecent; ++j) {
        recentFileActions[j]->setVisible(false);
    }

    if (numRecentFiles == 0) {
        for (auto &action : recentMenu->actions()) {
            action->setVisible(false);
        }
    }
}

void ApplicationMenu::createRecentFilesActions() {
    for (int i = 0; i < FileSystemIOManager::Settings::numberOfRecent; ++i) {
        auto *action = new QAction();
        action->setVisible(false);
        connect(action,
                &QAction::triggered,
                window,
                &MainWindow::openRecentFile);

        recentFileActions.append(action);
    }
}
