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
#include "filesystemiomanager.h"
#include <memory>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool createEmpty = false, QWidget *parent = nullptr);

    void updateRecentFileActions();
private:
    QScrollArea *slotBoardScrollArea;
    SlotBoard *slotBoard;
    std::unique_ptr<Strategy> strategy;
    QStackedWidget *stackedWidget;
    ActivitiesListWidget *activitiesListWidget;
    ActivityEditor *activityEditorWidget;
    std::optional<Activity> activityBeingEdited;

    void createActions();
    void createMenus();
    void newWindow();
    void open();
    void save();
    void saveAs();
    void openRecentFile();
    void load(QString path);
    void openActivityEditor();
    void updateWindowTitle(bool isSaved = true);
    void activityEdited(const Activity &activity, bool isNew);
    void removeActivityFromSlots(const Activity &activity);
    void editActivity(const Activity &activity);

    void showActivitiesListForSelection(QVector<int> selection);
    void setActivity(const Activity &activity);

    void setStrategy(Strategy *newStrategy);

    Notifier *notifier;
    std::unique_ptr<FileSystemIOManager> fsIOManager;

    QVector<QAction *> recentFileActions;
};

#endif // MAINWINDOW_H
