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

MainWindow::MainWindow(bool createEmtpty, QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(350);
    resize(350, QDesktopWidget().availableGeometry(this).size().height() * 0.8);

    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);

    fsIOManager = new FileSystemIOManager(this);

    auto lastOpened = fsIOManager->lastOpened();
    if (!createEmtpty && lastOpened.has_value()) {
        strategy = new Strategy(lastOpened.value());
    } else {
        strategy = Strategy::createEmpty();
        fsIOManager->resetFilepath();
    }

    activitiesListWidget = new ActivitiesListWidget();
    activitiesListWidget->setStrategy(strategy);

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

    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(slotBoardScrollArea);
    stackedWidget->addWidget(activitiesListWidget);

    activityEditorWidget = new ActivityEditor();
    connect(activityEditorWidget,
            &ActivityEditor::done,
            this,
            &MainWindow::activityEdited);

    stackedWidget->addWidget(activityEditorWidget);

    slotBoard = new SlotBoard();

    slotBoardScrollArea->setWidget(slotBoard);
    slotBoardScrollArea->setMouseTracking(true);


    slotBoard->setStrategy(strategy);
    connect(slotBoard->groupsList(),
            &GroupsList::wantToSetActivtyForSelection,
            this,
            &MainWindow::showActivitiesListForSelection);

    setCentralWidget(stackedWidget);
    notifier = new Notifier(strategy, this);

    updateWindowTitle();

    createMenus();
}

MainWindow::~MainWindow()
{
    delete strategy;
}

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

void MainWindow::setStrategy(Strategy *strategy)
{
    this->strategy = strategy;
    slotBoard->setStrategy(strategy);
    activitiesListWidget->setStrategy(strategy);
    notifier->setStrategy(strategy);
    updateWindowTitle();
}

