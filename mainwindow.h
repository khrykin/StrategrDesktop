#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QStackedWidget>

#include "activitieslistwidget.h"
#include "filesystemiomanager.h"
#include "notifier.h"
#include "slotboard.h"
#include "strategy.h"
#include "strategysettings.h"
#include <memory>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(bool createEmpty = false, QWidget *parent = nullptr);

  void updateRecentFileActions();
  void updateWindowTitle(bool isSaved = true);

  QStackedWidget *stackedWidget() const;

  QScrollArea *slotBoardScrollArea() const;

private:
  QScrollArea *_slotBoardScrollArea;
  SlotBoard *slotBoard;
  std::unique_ptr<Strategy> strategy;
  QStackedWidget *_stackedWidget;
  ActivitiesListWidget *activitiesListWidget;
  StrategySettings *strategySettingsWidget;
  std::optional<Activity> activityBeingEdited;

  QMenu *recentMenu = nullptr;
  QMenu *editMenu = nullptr;

  void createActions();
  void createMenus();
  void createSlotBoard();
  void createActivitiesListWidget();
  void createActivityEditorWidget();
  void createStackedWidget();
  void createStrategySettingsWidget();

  void newWindow();
  void open();
  void save();
  void saveAs();
  void openRecentFile();
  void clearRecent();
  void load(QString path);
  void openStrategySettings();
  void activityEdited(const Activity &activity, bool isNew);
  void removeActivityFromSlots(const Activity &activity);
  void appendActivity(const Activity &activity);

  void showActivitiesListForSelection(QVector<int> selection);
  void setActivity(const Activity &activity);
  void editActivityAtIndex(int index, const Activity &activity);
  void saveCurrentActivitiesAsDefault();
  void saveCurrentStrategyAsDefault();

  void selectAllSlots();

  void setStrategy(Strategy *newStrategy);
  void updateUI();

  bool showAreYouSureDialog(FileSystemIOManager *fsIOManager);

  Strategy *openRecentOrNew(bool forceNew = false);
  Strategy *openDefaultOrNew();

  Notifier *notifier;
  std::unique_ptr<FileSystemIOManager> fsIOManager;

  QVector<QAction *> recentFileActions;
};

#endif // MAINWINDOW_H
