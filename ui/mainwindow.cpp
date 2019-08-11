//
// Created by Dmitry Khrykin on 2019-07-08.
//
#include <iostream>

#include "mainwindow.h"
#include "application.h"
#include "alert.h"
#include "macoswindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setup();
}

MainWindow::MainWindow(FileSystemIOManager fsIOManager, QWidget *parent)
        : fsIOManager(std::move(fsIOManager)),
          strategy(this->fsIOManager.lastOpened()),
          QMainWindow(parent) {
    this->fsIOManager.setWindow(this);

    if (!this->fsIOManager.fileInfo().filePath().isEmpty()) {
        Application::openedFiles.append(this->fsIOManager.fileInfo().filePath());
    }

    setup();
}

void MainWindow::setup() {
    WindowGeometryManager::setInitialGeometry(this);

#ifdef Q_OS_MAC
    MacOSWindow::setup(this);
#endif

    strategy->setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    _scene = new MainScene(strategy.get(), this);
    _menu = new ApplicationMenu(this);

    setCentralWidget(_scene);

    updateWindowTitle();

    fsIOManager.setIsSaved(true);
}

MainWindow::~MainWindow() {
    teardown();
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
    auto newFsIOManger = FileSystemIOManager(fsIOManager);
    auto filePath = newFsIOManger.getOpenFileName();

    loadFile(filePath);
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
    auto isSaved = !fsIOManager.fileInfo().baseName().isEmpty();
    setIsSaved(isSaved);
}

void MainWindow::setIsSaved(bool isSaved) {
    fsIOManager.setIsSaved(isSaved);

    auto strategyTitle = fsIOManager.fileInfo().baseName();

    if (strategyTitle.isEmpty()) {
        strategyTitle = "Untitled";
    }

    auto windowTitle = strategyTitle + (isSaved ? "" : "*");

    setWindowTitle(windowTitle);

#ifdef Q_OS_MAC
    MacOSWindow::updateWindowTitle(this);
#endif
}

void MainWindow::loadFile(const QString &path, bool inNewWindow) {
    auto newFsIOManger = FileSystemIOManager(fsIOManager);
    auto loadedStrategy = newFsIOManger.read(path);
    if (!loadedStrategy) {
        qWarning() << "Strategy wasn't loaded";
        return;
    }

    if (Application::openedFiles.indexOf(newFsIOManger.fileInfo().filePath()) >= 0) {
        return;
    }

    if (inNewWindow) {
        auto *newWindow = new MainWindow();
        newWindow->fsIOManager = newFsIOManger;
        newWindow->setStrategy(std::move(loadedStrategy));
        newWindow->show();
    } else {
        Application::openedFiles.removeAll(fsIOManager.fileInfo().filePath());
        fsIOManager = newFsIOManger;
        setStrategy(std::move(loadedStrategy));
    }
}

void MainWindow::setStrategy(std::unique_ptr<Strategy> newStrategy) {
    strategy = std::move(newStrategy);
    strategy->setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    Application::openedFiles.append(fsIOManager.fileInfo().filePath());

    setIsSaved(true);

    updateUI();
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
        teardown();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::teardown() {
    WindowGeometryManager::saveGeometry(this);
    Application::openedFiles.removeAll(fsIOManager.fileInfo().filePath());
}

MainWindow *MainWindow::createLastOpened() {
    auto fsIOManager = FileSystemIOManager(nullptr);
    auto window = new MainWindow(fsIOManager);

    return window;
}

void MainWindow::updateUI() {
    scene()->setStrategy(strategy.get());
}
