//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <algorithm>

#include <QAction>
#include <QMessageBox>
#include <QSettings>

#include "application.h"
#include "applicationmenu.h"
#include "mainwindow.h"
#include "aboutwindow.h"
#include "mainscene.h"
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

    editMenu->addAction(tr("Undo"),
                        [=]() {
                            window->strategy.undo();
                        },
                        QKeySequence(Qt::CTRL + Qt::Key_Z));

    editMenu->addAction(tr("Redo"),
                        [=]() {
                            window->strategy.redo();
                        },
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));

    editMenu->addSeparator();

    editMenu->addAction(tr("Reorder Activities By Usage"),
                        window->scene(),
                        &MainScene::reorderActivitiesByUsage,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));

}

void ApplicationMenu::setupViewMenu() {
    addMenu(viewMenu);

    viewMenu->addSeparator();

    viewMenu->addAction(tr("Activities"),
                        window->scene(),
                        &MainScene::showActivities,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));

    viewMenu->addSeparator();

    viewMenu->addAction(tr("Go to Current Time"),
                        window->scene(),
                        &MainScene::focusOnCurrentTime,
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
        auto optionsWasSet = !Application::currentSettings().value("calendarExportOptions").isNull();
        auto initialOptions = optionsWasSet
                              ? Application::currentSettings().value("calendarExportOptions").toUInt()
                              : MacOSCalendarExporter::defaultOptions;

        auto initialCalendarTitle = Application::currentSettings().value(
                "exportCalendarTitle").toString().toStdString();

        auto[result, options, date, calendarTitle] = MacOSCalendarExporter::showExportOptionsWindow(initialOptions,
                                                                                                    initialCalendarTitle);
        if (result == MacOSCalendarExporter::Response::Perform) {
            Application::currentSettings().setValue("calendarExportOptions", options);

            if (!calendarTitle.empty())
                Application::currentSettings().setValue("exportCalendarTitle", QString::fromStdString(calendarTitle));

            MacOSCalendarExporter::exportStrategy(window->strategy, options, date, calendarTitle);
        }
    });
#endif
}

void ApplicationMenu::addImportFromCalendarAction() {
#ifdef Q_OS_MAC
    fileMenu->addAction(tr("Import From Calendar"), [this]() {
        auto optionsWasSet = !Application::currentSettings().value("calendarImportOptions").isNull();
        auto initialOptions = optionsWasSet
                              ? Application::currentSettings().value("calendarImportOptions").toUInt()
                              : MacOSCalendarExporter::defaultOptions;

        std::unique_ptr<std::vector<std::string>> initialCalendarIdentifiers = nullptr;

        auto initialCalendarIdentifiersWasSet = !Application::currentSettings().value(
                "importCalendarsIdentifiers").isNull();
        if (initialCalendarIdentifiersWasSet) {
            auto qInitialCalendarIdentifiers = Application::currentSettings().value(
                    "importCalendarsIdentifiers").toString().split(";");

            initialCalendarIdentifiers = std::make_unique<std::vector<std::string>>(
                    !qInitialCalendarIdentifiers.isEmpty()
                    ? QStringListToStdVector(qInitialCalendarIdentifiers)
                    : std::vector<std::string>()
            );
        }

        auto[result, options, date, calendarsIdentifiers] = MacOSCalendarExporter::showImportOptionsWindow(
                initialOptions,
                std::move(initialCalendarIdentifiers));

        if (result == MacOSCalendarExporter::Response::Perform) {
            Application::currentSettings().setValue("calendarImportOptions", options);

            if (calendarsIdentifiers) {
                auto qCalendarsIdentifiers = QStringListFromStdVector(*calendarsIdentifiers);
                Application::currentSettings().setValue("importCalendarsIdentifiers", qCalendarsIdentifiers.join(";"));
            } else {
                Application::currentSettings().remove("importCalendarsIdentifiers");
            }

            MacOSCalendarExporter::importStrategy(window->strategy, options, date, std::move(calendarsIdentifiers));
        }
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
