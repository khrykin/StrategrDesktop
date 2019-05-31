#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QStackedWidget>

#include "slotboard.h"
#include "strategy.h"
#include "activitieslistwidget.h"
#include "activityeditor.h"
#include "notifier.h"

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
    std::optional<Activity> activityBeingEdited;

    void createMenus();
    void newWindow();
    void open();
    void save();
    void saveAs();
    void openActivityEditor();
    void activityEdited(const Activity &activity, bool isNew);
    void removeActivityFromSlots(const Activity &activity);
    void editActivity(const Activity &activity);

    void showActivitiesListForSelection(QVector<int> selection);
    void setActivity(const Activity &activity);

    Notifier *notifier;
};

#endif // MAINWINDOW_H
