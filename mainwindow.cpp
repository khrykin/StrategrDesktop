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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumWidth(300);
    setMaximumWidth(350);
    resize(350, QDesktopWidget().availableGeometry(this).size().height() * 0.8);

    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);

    strategy = Strategy::createEmtpty();
    strategy->title = "Morning Training";

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

    if (strategy->title.has_value()) {
        setWindowTitle(QString::fromStdString(strategy->title.value()));
    }

    slotBoard->setStrategy(strategy);
    connect(slotBoard->groupsList(),
            &GroupsList::wantToSetActivtyForSelection,
            this,
            &MainWindow::showActivitiesListForSelection);

    setCentralWidget(stackedWidget);
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
    qDebug() << "open";
}

void MainWindow::newWindow()
{
    auto *newWindow =new MainWindow();
    newWindow->show();
    newWindow->setWindowTitle("Empty Strategy");
}

void MainWindow::save()
{
    qDebug() << "save";
}


void MainWindow::saveAs()
{
    qDebug() << "save as";
}

void MainWindow::openActivityEditor()
{
    stackedWidget->setCurrentIndex(2);
    activityEditorWidget->reset();
}

void MainWindow::activityEdited(const Activity &activity, bool isNew)
{
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

