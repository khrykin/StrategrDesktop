#include "mainwindow.h"
#include <QLayout>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMenu>
#include <functional>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>

#include "jsonserializer.h"

MainWindow::MainWindow(bool createEmpty, QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(350);
    resize(350, QDesktopWidget().availableGeometry(this).size().height() * 0.8);
    fsIOManager = std::make_unique<FileSystemIOManager>(this);

    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);
    slotBoardScrollArea->setMouseTracking(true);

    slotBoard = new SlotBoard();
    slotBoardScrollArea->setWidget(slotBoard);

    createActions();
    createMenus();

    auto lastOpened = fsIOManager->lastOpened();
    if (!createEmpty && lastOpened.has_value()) {
        strategy = std::make_unique<Strategy>(lastOpened.value());
    } else {
        strategy = std::unique_ptr<Strategy>(Strategy::createEmpty());
        fsIOManager->resetFilepath();
    }

    notifier = new Notifier(strategy.get(), this);

    activitiesListWidget = new ActivitiesListWidget();
    activitiesListWidget->setStrategy(strategy.get());

    connect(activitiesListWidget,
            &ActivitiesListWidget::selectActivity,
            this,
            &MainWindow::setActivity);

    connect(activitiesListWidget,
            &ActivitiesListWidget::wantNewActivity,
            this,
            &MainWindow::openActivityEditor);

    connect(activitiesListWidget,
            &ActivitiesListWidget::activityRemoved,
            this,
            &MainWindow::removeActivityFromSlots);

    connect(activitiesListWidget,
            &ActivitiesListWidget::wantToEditActivity,
            this,
            &MainWindow::editActivity);

    activityEditorWidget = new ActivityEditor();
    connect(activityEditorWidget,
            &ActivityEditor::done,
            this,
            &MainWindow::activityEdited);

    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(slotBoardScrollArea);
    stackedWidget->addWidget(activitiesListWidget);
    stackedWidget->addWidget(activityEditorWidget);


    slotBoard->setStrategy(strategy.get());

    connect(slotBoard->groupsList(),
            &GroupsList::wantToSetActivtyForSelection,
            this,
            &MainWindow::showActivitiesListForSelection);

    setCentralWidget(stackedWidget);
    updateWindowTitle();
}

//MainWindow::~MainWindow()
//{
//    QWidget::~QWidget();
////    delete strategy;
//}

void MainWindow::createMenus()
{
    auto *menuBar = new QMenuBar();
    auto *fileMenu = new QMenu("File");
    auto *editMenu = new QMenu("Edit");
    auto *notificationsMenu = new QMenu("Notifications");

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(editMenu);
    menuBar->addMenu(notificationsMenu);

    fileMenu->addAction("New",
                        this,
                        &MainWindow::newWindow,
                        QKeySequence(Qt::CTRL + Qt::Key_N));

    fileMenu->addAction("Open",
                        this,
                        &MainWindow::open,
                        QKeySequence(Qt::CTRL + Qt::Key_O));

    fileMenu->addAction("Save",
                        this,
                        &MainWindow::save,
                        QKeySequence(Qt::CTRL + Qt::Key_S));

    fileMenu->addAction("Save As",
                        this,
                        &MainWindow::saveAs,
                        QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));

    for (int i = 0; i < FileSystemIOManager::Settings::numberOfRecent; ++i) {
        fileMenu->addAction(recentFileActions[i]);
    }

    fileMenu->addSeparator();
    updateRecentFileActions();

    setMenuBar(menuBar);

    editMenu->addAction(slotBoard->groupsList()->getUndoAction());
    editMenu->addAction(slotBoard->groupsList()->getRedoAction());
}


void MainWindow::open()
{
    auto newStrategy = fsIOManager->open();
    if (!newStrategy.has_value()) {
        qDebug() << "Can't open";
        return;
    }

    setStrategy(new Strategy(newStrategy.value()));
}

void MainWindow::newWindow()
{
    auto *newWindow =new MainWindow(true);
    newWindow->show();
}

void MainWindow::save()
{
    fsIOManager->save(*strategy);
    updateWindowTitle();
}


void MainWindow::saveAs()
{
    fsIOManager->saveAs(*strategy);
    updateWindowTitle();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        load(action->data().toString());
    }
}

void MainWindow::load(QString path)
{
    auto loadedStrategy = fsIOManager->read(path);
    if (!loadedStrategy.has_value()) {
        qDebug() << "Can't open";
        return;
    }

    setStrategy(new Strategy(loadedStrategy.value()));
}

void MainWindow::openActivityEditor()
{
    stackedWidget->setCurrentIndex(2);
    activityEditorWidget->reset();
}

void MainWindow::updateWindowTitle(bool isSaved)
{
    auto title = fsIOManager->fileInfo().baseName();
    qDebug() << "baseName" << title;
    qDebug() << "filename" << fsIOManager->fileInfo().fileName();
    auto currentTitle = title.isEmpty()
            ? "Untitled"
            : title  + (isSaved ? "" : "*");

    setWindowTitle(currentTitle + " - Strategr");
}

void MainWindow::activityEdited(const Activity &activity, bool isNew)
{
    updateWindowTitle(false);

    if (!isNew && activityBeingEdited.has_value()) {
        strategy->editActivity(activityBeingEdited.value(), activity);
        stackedWidget->setCurrentIndex(1);
        activitiesListWidget->updateList();
        slotBoard->groupsList()->updateUI();
        activityBeingEdited = std::nullopt;
        return;
    }

    if (!strategy->hasActivity(activity)) {
        strategy->appendActivity(activity);
        stackedWidget->setCurrentIndex(1);
        activitiesListWidget->updateList();
    } else {
        activityEditorWidget->showError("Name", "Already exists");
    }

}

void MainWindow::removeActivityFromSlots(const Activity &activity)
{
    slotBoard->groupsList()->updateUI();
}

void MainWindow::editActivity(const Activity &activity)
{
    activityEditorWidget->setActivity(activity);
    stackedWidget->setCurrentIndex(2);
    activityBeingEdited = activity;
}

void MainWindow::showActivitiesListForSelection(QVector <int> selection)
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::setActivity(const Activity &activity)
{
    auto selection = slotBoard->groupsList()->selectionSlots();
    strategy->setSlotAtIndices(selection.toStdVector(), activity);
    slotBoard->groupsList()->deselectAllSlots();
    stackedWidget->setCurrentIndex(0);
    slotBoard->groupsList()->updateUI();
}

void MainWindow::setStrategy(Strategy *newStrategy)
{
    this->strategy = std::unique_ptr<Strategy>(newStrategy);
    slotBoard->setStrategy(newStrategy);
    activitiesListWidget->setStrategy(newStrategy);
    notifier->setStrategy(newStrategy);
    updateWindowTitle();
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    auto files = fsIOManager->recentPahts();

    int numRecentFiles = qMin(files.size(), FileSystemIOManager::Settings::numberOfRecent);
    auto recentFileNames = fsIOManager->recentFileNames();

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(recentFileNames[i]);
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(files[i]);
        recentFileActions[i]->setVisible(true);
    }

    for (int j = numRecentFiles; j < FileSystemIOManager::Settings::numberOfRecent; ++j)
        recentFileActions[j]->setVisible(false);
}

void MainWindow::createActions()
{
    for (int i = 0; i < FileSystemIOManager::Settings::numberOfRecent; ++i) {
        auto *action = new QAction(this);
        action->setVisible(false);
        connect(action, &QAction::triggered,
                this, &MainWindow::openRecentFile);
        recentFileActions.append(action);
    }
}

