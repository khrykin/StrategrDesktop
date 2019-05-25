#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QStackedWidget>

#include "slotboard.h"
#include "strategy.h"
#include "activitieslistwidget.h"
#include "activityeditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QScrollArea *slotBoardScrollArea;
    SlotBoard *slotBoard;
    Strategy *strategy;
    QStackedWidget *stackedWidget;
    ActivitiesListWidget *activitiesListWidget;
    ActivityEditor *activityEditorWidget;

    void createMenus();
    void newWindow();
    void open();
    void save();
    void saveAs();
    void openActivityEditor();
    void activityEdited(const Activity &activity);
    void removeActivityFromSlots(const Activity &activity);

    void showActivitiesListForSelection(QVector<int> selection);
    void setActivity(const Activity &activity);


};

#endif // MAINWINDOW_H
