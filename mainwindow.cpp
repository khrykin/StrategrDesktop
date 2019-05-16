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

#include "activitieslistwidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumWidth(250);
    resize(300, QDesktopWidget().availableGeometry(this).size().height() * 0.7);

    slotBoardScrollArea = new QScrollArea();
    slotBoardScrollArea->setWidgetResizable(true);

    strategy = Strategy::createEmtpty();
    strategy->title = "Morning Training";

    auto *activitiesListWidget = new ActivitiesListWidget();
    activitiesListWidget->setStrategy(strategy);
    connect(activitiesListWidget,
            &ActivitiesListWidget::selectActivity,
            this,
            &MainWindow::setActivity);

    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(slotBoardScrollArea);
    stackedWidget->addWidget(activitiesListWidget);

    slotBoard = new SlotBoard();

    slotBoardScrollArea->setWidget(slotBoard);
    slotBoardScrollArea->setMouseTracking(true);

    if (strategy->title.has_value()) {
        setWindowTitle(QString::fromStdString(strategy->title.value()));
    }

    slotBoard->setStrategy(strategy);
    connect(slotBoard,
            &SlotBoard::wantToSetActivtyForSelection,
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

    menuBar->addMenu(fileMenu);
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

void MainWindow::showActivitiesListForSelection(QVector <int> selection)
{
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::setActivity(std::shared_ptr<Activity> activity)
{
    auto selection = slotBoard->selectionSlots();
    strategy->setSlotAtIndices(selection.toStdVector(), activity);
    slotBoard->deselectAllSlots();
    stackedWidget->setCurrentIndex(0);
    slotBoard->updateUI();
}

