//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    WindowGeometryManager::setInitialGeometry(this);

    _scene = new MainScene(strategy, this);
    _menu = new ApplicationMenu(this);
    // TODO Notifier

    setCentralWidget(_scene);

    updateWindowTitle();
}


MainWindow::~MainWindow() {
    WindowGeometryManager::saveGeometry(this);
}

Strategy MainWindow::openRecentOrCreateNew() {
    auto lastOpened = fsIOManager.lastOpened();
    updateWindowTitle();

    auto openedStrategy = lastOpened
                          ? *lastOpened
                          : fsIOManager.openDefaultStrategy();

    openedStrategy.setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    return openedStrategy;
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
        auto wantToDiscard = oldFsIOManger.askIfWantToDiscard(strategy);
        if (wantToDiscard) {
            return;
        }
    }

    setStrategy(*newStrategy);
}

void MainWindow::saveFile() {
    fsIOManager.save(strategy);
    setIsSaved(fsIOManager.isSaved());
}

void MainWindow::saveFileAs() {
    fsIOManager.saveAs(strategy);
    setIsSaved(fsIOManager.isSaved());
}

void MainWindow::saveCurrentStrategyAsDefault() {
    fsIOManager.saveAsDefault(strategy);
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
    setIsSaved(true);
}

void MainWindow::setIsSaved(bool isSaved) {
    fsIOManager.setIsSaved(isSaved);

    auto title = fsIOManager.fileInfo().baseName();
    auto currentTitle = title.isEmpty()
                        ? "Untitled"
                        : title + (isSaved ? "" : "*");

    setWindowTitle(currentTitle);
}

void MainWindow::loadFile(const QString &path) {
    auto loadedStrategy = fsIOManager.read(path);
    if (!loadedStrategy) {
        return;
    }

    setStrategy(*loadedStrategy);
}

void MainWindow::setStrategy(const Strategy &newStrategy) {
    strategy = newStrategy;
    strategy.setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    setIsSaved(true);

    scene()->setStrategy(strategy);
}

void MainWindow::strategyStateChanged() {
    setIsSaved(false);
}

bool MainWindow::wantToClose() {
    auto wantToClose = true;

    if (!fsIOManager.isSaved()) {
        wantToClose = fsIOManager.askIfWantToDiscard(strategy);
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



