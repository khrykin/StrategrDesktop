#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>

#include "slotboard.h"
#include "strategy.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QScrollArea *scrollArea;
    SlotBoard *slotBoard;
    Strategy *strategy;

    void createMenus();

    void newWindow();
    void open();
    void save();
    void saveAs();
};

#endif // MAINWINDOW_H
