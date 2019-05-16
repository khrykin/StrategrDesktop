#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QStackedWidget>

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
    QStackedWidget *stackedWidget;

    void createMenus();

    void newWindow();
    void open();
    void save();
    void saveAs();

    void showActivitiesListForSelection(QVector<int> selection);
    void setActivity(std::shared_ptr<Activity> activity);
};

#endif // MAINWINDOW_H
