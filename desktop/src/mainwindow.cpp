//
// Created by Dmitry Khrykin on 2019-07-08.
//

#include <iostream>

#include <QCloseEvent>

#include "alert.h"
#include "application.h"
#include "applicationmenu.h"
#include "macoswindow.h"
#include "mainscene.h"
#include "mainwindow.h"

MainWindow::MainWindow(const QString &filePath, QWidget *parent)
    : strategy(fsIOManager.openFromPathOrDefault(filePath)),
      QMainWindow(parent) {
    setup();
}

MainWindow::MainWindow(QWidget *parent)
    : strategy(fsIOManager.openDefault()),
      QMainWindow(parent) {
    setup();
}

MainWindow::MainWindow(FileSystemIOManager existingFsIOManager, QWidget *parent)
    : QMainWindow(parent),
      fsIOManager(std::move(existingFsIOManager)),
      strategy(fsIOManager.openLastOrDefault()) {
    fsIOManager.setWindow(this);
    setup();
}

void MainWindow::setup() {
    WindowGeometryManager::setInitialGeometry(this);

    if (!fsIOManager.fileInfo().filePath().isEmpty()) {
        Application::registerOpenedFile(fsIOManager.fileInfo().filePath());
    }

#ifdef Q_OS_MAC
    MacOSWindow::setup(this);
#endif

    strategy.add_on_change_callback(this, &MainWindow::strategyStateChanged);

    _scene = new MainScene(strategy, this);
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
    Application::clearRecentFiles();
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
    if (strategyTitle.isEmpty())
        strategyTitle = "Untitled";

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

void MainWindow::setStrategy(const stg::strategy &newStrategy) {
    strategy = newStrategy;
    strategy.add_on_change_callback(this, &MainWindow::strategyStateChanged);

    Application::registerOpenedFile(fsIOManager.fileInfo().filePath());

    setIsSaved(true);

    reloadStrategy();
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
    if (alreadyTornDown)
        return;

    WindowGeometryManager::saveGeometry(this);
    Application::markFileClosed(fsIOManager.fileInfo().filePath());
    alreadyTornDown = true;
}

MainWindow *MainWindow::createLastOpened() {
    auto fsIOManager = FileSystemIOManager(nullptr);
    auto *window = new MainWindow(fsIOManager);

    return window;
}

int MainWindow::toolbarHeight() {
#ifdef Q_OS_MAC
    return (int) MacOSWindow::toolbarHeight(this);
#else
    return 0;
#endif
}

void MainWindow::reloadStrategy() {
    scene()->reloadStrategy();
}
