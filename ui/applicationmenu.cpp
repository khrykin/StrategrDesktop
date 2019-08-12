//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <QAction>
#include <QMessageBox>

#include "applicationsettings.h"
#include "applicationmenu.h"
#include "mainwindow.h"
#include "aboutwindow.h"

ApplicationMenu::ApplicationMenu(MainWindow *window) : window(window) {
    setupFileMenu();
    setupEditMenu();
    setupViewMenu();

    addMenu(helpMenu);
    helpMenu->addAction("About", [=]() {
        auto aboutWindow = new AboutWindow(window);
        aboutWindow->show();
    });

    window->setMenuBar(this);
}

void ApplicationMenu::setupEditMenu() {
    addMenu(editMenu);

    editMenu->addAction(tr("Undo"),
                        [=]() {
                            window->strategy->undo();
                        },
                        QKeySequence(Qt::CTRL + Qt::Key_Z));

    editMenu->addAction(tr("Redo"),
                        [=]() {
                            window->strategy->redo();
                        },
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
}

void ApplicationMenu::setupViewMenu() {
    addMenu(viewMenu);

    viewMenu->addSeparator();

    viewMenu->addAction(tr("Edit Activities"),
                        window->scene(),
                        &MainScene::showActivities,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));

    viewMenu->addSeparator();

    viewMenu->addAction(tr("Go to Current Time"),
                        window->scene(),
                        &MainScene::focusOnCurrentTime,
                        QKeySequence(Qt::CTRL + Qt::Key_Slash));

    viewMenu->addSeparator();
}

void ApplicationMenu::setupFileMenu() {
    addMenu(fileMenu);

    fileMenu->addAction(tr("New"),
                        window,
                        &MainWindow::openNewWindow,
                        QKeySequence(Qt::CTRL + Qt::Key_N));

    fileMenu->addAction(tr("Open"),
                        window,
                        &MainWindow::openFile,
                        QKeySequence(Qt::CTRL + Qt::Key_O));

    setupRecentMenu();

    fileMenu->addSeparator();

    fileMenu->addAction(tr("Save"),
                        window,
                        &MainWindow::saveFile,
                        QKeySequence(Qt::CTRL + Qt::Key_S));

    fileMenu->addAction(tr("Save As"),
                        window,
                        &MainWindow::saveFileAs,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));

    fileMenu->addSeparator();

    fileMenu->addAction(tr("Close"),
                        window,
                        &MainWindow::close,
                        QKeySequence(Qt::CTRL + Qt::Key_W));

    fileMenu->addSeparator();

    fileMenu->addAction(tr("Settings"),
                        window->scene(),
                        &MainScene::showStrategySettings,
                        QKeySequence(Qt::CTRL + Qt::Key_Comma));

    fileMenu->addAction(tr("Save as Default"),
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

    recentMenu->addAction(tr("Clear List"),
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
