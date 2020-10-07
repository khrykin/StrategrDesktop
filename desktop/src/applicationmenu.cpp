//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <algorithm>

#include <QAction>
#include <QMessageBox>
#include <QSettings>

#include "aboutwindow.h"
#include "application.h"
#include "applicationmenu.h"
#include "mainscene.h"
#include "mainwindow.h"
#include "utils.h"

#ifdef Q_OS_MAC

#include "macoscalendarexporter.h"
#include "mainscene.h"

#endif

ApplicationMenu::ApplicationMenu(MainWindow *window) : window(window) {
    setupFileMenu();
    setupEditMenu();
    setupViewMenu();

    addMenu(helpMenu);

    auto *aboutAction = helpMenu->addAction(tr("About"), [=] {
        if (!Application::aboutWindow) {
            Application::aboutWindow = new AboutWindow(window);
            Application::aboutWindow->show();
        }

        Application::aboutWindow->raise();
    });

    aboutAction->setMenuRole(QAction::AboutRole);

    //    if (!Application::updateMenuAdded) {
    auto *updateAction = helpMenu->addAction(tr("Check For Updates..."), [=] {
        Application::checkForUpdates();
    });

    updateAction->setMenuRole(QAction::ApplicationSpecificRole);

    Application::updateMenuAdded = true;
    //    }

    window->setMenuBar(this);
}

void ApplicationMenu::setupEditMenu() {
    addMenu(editMenu);

    addAction(editMenu,
              actionCenter().undo,
              QKeySequence(Qt::CTRL + Qt::Key_Z));

    addAction(editMenu,
              actionCenter().redo,
              QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));

    editMenu->addSeparator();

    addAction(editMenu,
              actionCenter().reorder_activities_by_usage,
              QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
}

void ApplicationMenu::setupViewMenu() {
    addMenu(viewMenu);

    viewMenu->addSeparator();

    addAction(viewMenu,
              actionCenter().show_activities,
              QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));

    viewMenu->addSeparator();

    addAction(viewMenu,
              actionCenter().go_to_current_time,
              QKeySequence(Qt::CTRL + Qt::Key_Slash));
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

    fileMenu->addSeparator();

    addImportFromCalendarAction();
    addExportToCalendarAction();

    fileMenu->addSeparator();
}

void ApplicationMenu::addExportToCalendarAction() {
#ifdef Q_OS_MAC
    fileMenu->addAction(tr("Export To Calendar"), [this]() {
        MacOSCalendarExporter::exportStrategy(window->strategy);
    });
#endif
}

void ApplicationMenu::addImportFromCalendarAction() {
#ifdef Q_OS_MAC
    fileMenu->addAction(tr("Import From Calendar"), [this]() {
        MacOSCalendarExporter::importStrategy(window->strategy);
    });
#endif
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
        auto text = recentFileNames[i];
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

MainScene *ApplicationMenu::mainScene() {
    return window->scene();
}

stg::action_center &ApplicationMenu::actionCenter() {
    return mainScene()->actionCenter();
}

QAction *ApplicationMenu::addAction(QMenu *menu, const stg::action &action, const QKeySequence &shortcut) {
    return menu->addAction(QString::fromStdString(action.name), action, shortcut);
}
