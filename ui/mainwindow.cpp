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
          QMainWindow(parent) {
    if (FileSystemIOManager::lastOpenedFilePath()) {
        auto lastOpened = this->fsIOManager
                .read(*FileSystemIOManager::lastOpenedFilePath());
        if (lastOpened) {
            strategy = *lastOpened;
        } else {
            strategy = this->fsIOManager.openDefaultStrategy();
        }
    } else {
        strategy = this->fsIOManager.openDefaultStrategy();
    }

    this->fsIOManager.setWindow(this);

    if (!this->fsIOManager.fileInfo().filePath().isEmpty()) {
        Application::registerOpenedFile(this->fsIOManager.fileInfo().filePath());
    }

    setup();
}

void MainWindow::setup() {
    WindowGeometryManager::setInitialGeometry(this);

#ifdef Q_OS_MAC
    MacOSWindow::setup(this);
#endif

    strategy.setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    _scene = new MainScene(&strategy, this);
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
    fsIOManager.save(strategy);
    setIsSaved(fsIOManager.isSaved());
}

void MainWindow::saveFileAs() {
    auto oldFilePath = fsIOManager.fileInfo().filePath();

    fsIOManager.saveAs(strategy);
    setIsSaved(fsIOManager.isSaved());

    if (fsIOManager.isSaved()) {
        Application::markFileClosed(oldFilePath);
        Application::registerOpenedFile(fsIOManager.fileInfo().filePath());
    }
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
    auto isSaved = !fsIOManager.fileInfo().baseName().isEmpty();
    setIsSaved(isSaved);
}

void MainWindow::setIsSaved(bool isSaved) {
    fsIOManager.setIsSaved(isSaved);

    auto strategyTitle = fsIOManager.fileInfo().baseName();

    if (strategyTitle.isEmpty()) {
        strategyTitle = "Untitled";
    }

    setWindowModified(!isSaved);
    setWindowTitle(strategyTitle + "[*]");

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

    if (Application::fileIsOpened(newFsIOManger.fileInfo().filePath())) {
        return;
    }

    if (inNewWindow) {
        auto *newWindow = new MainWindow();
        newWindow->fsIOManager = newFsIOManger;
        newWindow->setStrategy(*loadedStrategy);
        newWindow->show();
    } else {
        Application::markFileClosed(fsIOManager.fileInfo().filePath());
        fsIOManager = newFsIOManger;
        setStrategy(*loadedStrategy);
    }
}

void MainWindow::setStrategy(const Strategy &newStrategy) {
    strategy = newStrategy;
    strategy.setOnChangeCallback(this, &MainWindow::strategyStateChanged);

    Application::registerOpenedFile(fsIOManager.fileInfo().filePath());

    setIsSaved(true);

    updateUI();
}

void MainWindow::strategyStateChanged() {
    setIsSaved(false);
}

bool MainWindow::wantToClose() {
    auto wantToClose = true;

    if (!fsIOManager.isSaved()) {
        wantToClose = !fsIOManager.askIfWantToDiscardOrLeaveCurrent(strategy);
    }

    return wantToClose;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (wantToClose()) {
        teardown();
        event->accept();
        this->deleteLater();
    } else {
        event->ignore();
    }
}

void MainWindow::teardown() {
    WindowGeometryManager::saveGeometry(this);
    Application::markFileClosed(fsIOManager.fileInfo().filePath());
}

MainWindow *MainWindow::createLastOpened() {
    auto fsIOManager = FileSystemIOManager(nullptr);
    auto window = new MainWindow(fsIOManager);

    return window;
}

void MainWindow::updateUI() {
    scene()->setStrategy(&strategy);
}
