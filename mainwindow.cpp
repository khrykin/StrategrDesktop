#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLayout>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMenu>
#include <functional>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    slotBoard = new SlotBoard();

    scrollArea->setWidget(slotBoard);
    scrollArea->setMouseTracking(true);

    strategy = Strategy::createEmtpty();
    strategy->title = "Morning Training";

    if (strategy->title.has_value()) {
        setWindowTitle(QString::fromStdString(strategy->title.value()));
    }

    slotBoard->setStrategy(strategy);

    slotBoard->updateUI();

    setCentralWidget(scrollArea);
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
