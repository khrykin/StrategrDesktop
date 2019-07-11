//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include "mainwindow.h"
#include <QMessageBox>

#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    WindowGeometryManager::setInitialGeometry(this);

    strategy->setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    _scene = new MainScene(strategy.get(), this);
    _menu = new ApplicationMenu(this);
    // TODO Notifier

    setCentralWidget(_scene);

    updateWindowTitle();
}


MainWindow::~MainWindow() {
    WindowGeometryManager::saveGeometry(this);
}

void MainWindow::loadRecent() {
    auto lastOpened = fsIOManager.lastOpened();
    if (lastOpened) {
        updateWindowTitle();
        setStrategy(std::move(*lastOpened));
    }
}

MainScene *MainWindow::scene() const {
    return _scene;
}

ApplicationMenu *MainWindow::menu() const {
    return _menu;
}

void MainWindow::openNewWindow() {
    auto *newWindow = new MainWindow();
    newWindow->show();
}

void MainWindow::openFile() {
    auto oldFsIOManger = FileSystemIOManager(fsIOManager);
    auto newStrategy = fsIOManager.open();

    if (!newStrategy) {
        return;
    }

    if (!oldFsIOManger.isSaved()) {
        auto wantToLeave = oldFsIOManger.askIfWantToDiscardOrLeaveCurrent(*strategy);
        if (wantToLeave) {
            return;
        }
    }

    setStrategy(std::move(newStrategy));
}

void MainWindow::saveFile() {
    fsIOManager.save(*strategy);
    setIsSaved(fsIOManager.isSaved());
}

void MainWindow::saveFileAs() {
    fsIOManager.saveAs(*strategy);
    setIsSaved(fsIOManager.isSaved());
}

void MainWindow::saveCurrentStrategyAsDefault() {
    fsIOManager.saveAsDefault(*strategy);
}

void MainWindow::clearRecentFilesList() {
    FileSystemIOManager::clearRecent();
    menu()->updateRecentFilesActions();
}

void MainWindow::openRecentFile() {
    auto *action = qobject_cast<QAction *>(sender());
    if (action) {
        loadFile(action->data().toString());
    }
}

void MainWindow::updateWindowTitle() {
    setIsSaved(false);
}

void MainWindow::setIsSaved(bool isSaved) {
    fsIOManager.setIsSaved(isSaved);

    auto strategyTitle = fsIOManager.fileInfo().baseName();

    if (strategyTitle.isEmpty()) {
        strategyTitle = "Untitled";
    }

    auto windowTitle = strategyTitle + (isSaved ? "" : "*");

    setWindowTitle(windowTitle);
}

void MainWindow::loadFile(const QString &path) {
    auto loadedStrategy = fsIOManager.read(path);
    if (!loadedStrategy) {
        return;
    }

    setStrategy(std::move(loadedStrategy));
}

void MainWindow::setStrategy(std::unique_ptr<Strategy> newStrategy) {
    strategy = std::move(newStrategy);
    strategy->setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    setIsSaved(true);

    scene()->setStrategy(strategy.get());
}

void MainWindow::strategyStateChanged() {
    setIsSaved(false);
}

bool MainWindow::wantToClose() {
    auto wantToClose = true;

    if (!fsIOManager.isSaved()) {
        wantToClose = !fsIOManager.askIfWantToDiscardOrLeaveCurrent(*strategy);
    }

    return wantToClose;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (wantToClose()) {
        WindowGeometryManager::saveGeometry(this);
        event->accept();
    } else {
        event->ignore();
    }
}



