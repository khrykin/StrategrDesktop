#include "mainwindow.h"
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTextEdit>
#include <functional>

#include "jsonserializer.h"

MainWindow::MainWindow(bool createEmpty, QWidget *parent)
    : QMainWindow(parent) {
  setMinimumWidth(300);
  //  setMaximumWidth(400);
  resize(400, QDesktopWidget().availableGeometry(this).size().height() * 1);

  fsIOManager = std::make_unique<FileSystemIOManager>(this);
  strategy = std::unique_ptr<Strategy>(openRecentOrNew(createEmpty));

  createActions();
  createMenus();

  notifier = new Notifier(strategy.get(), this);

  createSlotBoard();
  createActivitiesListWidget();
  createStrategySettingsWidget();
  createStackedWidget();

  setCentralWidget(_stackedWidget);
  updateWindowTitle();
}

// MainWindow::~MainWindow()
//{
//    QWidget::~QWidget();
////    delete strategy;
//}

void MainWindow::createMenus() {
  auto *menuBar = new QMenuBar();
  auto *fileMenu = new QMenu("File");
  editMenu = new QMenu("Edit");
  auto *notificationsMenu = new QMenu("Notifications");
  recentMenu = new QMenu("Open Recent");

  menuBar->addMenu(fileMenu);
  menuBar->addMenu(editMenu);
  menuBar->addMenu(notificationsMenu);

  fileMenu->addAction("New", this, &MainWindow::newWindow,
                      QKeySequence(Qt::CTRL + Qt::Key_N));
  fileMenu->addAction("Open", this, &MainWindow::open,
                      QKeySequence(Qt::CTRL + Qt::Key_O));

  fileMenu->addMenu(recentMenu);

  for (int i = 0; i < FileSystemIOManager::Settings::numberOfRecent; ++i) {
    recentMenu->addAction(recentFileActions[i]);
  }

  recentMenu->addSeparator();
  recentMenu->addAction("Clear List", this, &MainWindow::clearRecent);

  updateRecentFileActions();

  fileMenu->addSeparator();
  fileMenu->addAction("Save", this, &MainWindow::save,
                      QKeySequence(Qt::CTRL + Qt::Key_S));
  fileMenu->addAction("Save As", this, &MainWindow::saveAs,
                      QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));

  fileMenu->addSeparator();
  fileMenu->addAction("Settings", this, &MainWindow::openStrategySettings,
                      QKeySequence(Qt::CTRL + Qt::Key_Comma));
  fileMenu->addAction("Save Current Strategy as Default", this,
                      &MainWindow::saveCurrentStrategyAsDefault,
                      QKeySequence(Qt::CTRL + Qt::Key_D));
  setMenuBar(menuBar);
}

void MainWindow::createSlotBoard() {
  _slotBoardScrollArea = new QScrollArea();
  _slotBoardScrollArea->setWidgetResizable(true);
  _slotBoardScrollArea->setMouseTracking(true);
  _slotBoardScrollArea->setFrameShape(QFrame::NoFrame);

  slotBoard = new SlotBoard();
  _slotBoardScrollArea->setWidget(slotBoard);
  slotBoard->setStrategy(strategy.get());

  connect(slotBoard->groupsList(), &GroupsList::wantToSetActivtyForSelection,
          this, &MainWindow::showActivitiesListForSelection);

  connect(slotBoard->groupsList(), &GroupsList::wantToUpdateActivitiesList,
          [=]() { activitiesListWidget->updateList(); });

  connect(slotBoard->groupsList(), &GroupsList::slotsStateChanged,
          [=]() { updateWindowTitle(false); });

  editMenu->addAction(slotBoard->groupsList()->getUndoAction());
  editMenu->addAction(slotBoard->groupsList()->getRedoAction());
}

void MainWindow::createActivitiesListWidget() {
  activitiesListWidget = new ActivitiesListWidget();
  activitiesListWidget->setStrategy(strategy.get());

  connect(activitiesListWidget, &ActivitiesListWidget::selectActivity, this,
          &MainWindow::setActivity);

  connect(activitiesListWidget, &ActivitiesListWidget::activityRemoved, this,
          &MainWindow::removeActivityFromSlots);

  connect(activitiesListWidget, &ActivitiesListWidget::activityEditedAtIndex,
          this, &MainWindow::editActivityAtIndex);

  connect(activitiesListWidget, &ActivitiesListWidget::activityAppended, this,
          &MainWindow::appendActivity);
}

void MainWindow::createStackedWidget() {
  _stackedWidget = new QStackedWidget();
  _stackedWidget->addWidget(_slotBoardScrollArea);
  _stackedWidget->addWidget(activitiesListWidget);
  _stackedWidget->addWidget(strategySettingsWidget);
}

void MainWindow::createStrategySettingsWidget() {
  strategySettingsWidget = new StrategySettings();
  strategySettingsWidget->setStrategy(strategy.get());
  connect(strategySettingsWidget, &StrategySettings::strategySettingsUpdated,
          this, &MainWindow::updateUI);
}

void MainWindow::open() {
  auto oldFsIOManger = FileSystemIOManager(*fsIOManager.get());
  auto newStrategy = fsIOManager->open();
  if (!newStrategy) {
    // TODO: "Can't open";
    return;
  }

  if (!fsIOManager->isSaved()) {
    bool wantToContinue = showAreYouSureDialog(&oldFsIOManger);
    if (!wantToContinue) {
      return;
    }
  }

  setStrategy(new Strategy(newStrategy.value()));
}

void MainWindow::newWindow() {
  auto oldFsIOManger = FileSystemIOManager(*fsIOManager.get());
  auto *strategy = openRecentOrNew(true);
  if (!fsIOManager->isSaved()) {
    bool wantToContinue = showAreYouSureDialog(&oldFsIOManger);
    if (!wantToContinue) {
      return;
    }
  }

  setStrategy(strategy);
}

void MainWindow::save() {
  fsIOManager->save(*strategy);
  updateWindowTitle();
}

void MainWindow::saveAs() {
  fsIOManager->saveAs(*strategy);
  updateWindowTitle();
}

void MainWindow::openRecentFile() {
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    load(action->data().toString());
  }
}

void MainWindow::clearRecent() {
  fsIOManager->clearRecent();
  updateRecentFileActions();
}

void MainWindow::load(QString path) {
  auto loadedStrategy = fsIOManager->read(path);
  if (!loadedStrategy) {
    // TODO: "Can't open";
    return;
  }

  setStrategy(new Strategy(loadedStrategy.value()));
}

void MainWindow::openStrategySettings() {
  strategySettingsWidget->setGetBackTo(_stackedWidget->currentWidget());
  _stackedWidget->setCurrentWidget(strategySettingsWidget);
}

void MainWindow::updateWindowTitle(bool isSaved) {
  auto title = fsIOManager->fileInfo().baseName();

  fsIOManager->setIsSaved(isSaved);

  auto currentTitle =
      title.isEmpty() ? "Untitled" : title + (isSaved ? "" : "*");

  setWindowTitle(currentTitle);
}

QStackedWidget *MainWindow::stackedWidget() const { return _stackedWidget; }

QScrollArea *MainWindow::slotBoardScrollArea() const {
  return _slotBoardScrollArea;
}

void MainWindow::activityEdited(const Activity &activity, bool isNew) {
  updateWindowTitle(false);

  if (!isNew && activityBeingEdited) {
    strategy->editActivity(activityBeingEdited.value(), activity);
    _stackedWidget->setCurrentWidget(_slotBoardScrollArea);
    activitiesListWidget->updateList();
    slotBoard->groupsList()->updateUI();
    activityBeingEdited = std::nullopt;
    return;
  }

  if (!strategy->hasActivity(activity)) {
    strategy->appendActivity(activity);
    _stackedWidget->setCurrentWidget(_slotBoardScrollArea);
    activitiesListWidget->updateList();
  } else {
    //    activityEditorWidget->showError("Name", "Already exists");
  }
}

void MainWindow::removeActivityFromSlots(const Activity &activity) {
  updateWindowTitle(false);
  slotBoard->groupsList()->updateUI();
}

void MainWindow::appendActivity(const Activity &activity) {
  strategy->appendActivity(activity);
  activitiesListWidget->updateList();
  updateWindowTitle(false);
}

void MainWindow::showActivitiesListForSelection(QVector<int> selection) {
  _stackedWidget->setCurrentWidget(activitiesListWidget);
}

void MainWindow::setActivity(const Activity &activity) {
  auto selection = slotBoard->groupsList()->selectionSlots();
  strategy->setSlotAtIndices(selection.toStdVector(), activity);
  slotBoard->groupsList()->deselectAllSlots();
  _stackedWidget->setCurrentWidget(_slotBoardScrollArea);
  slotBoard->groupsList()->updateUI();
}

void MainWindow::editActivityAtIndex(int index, const Activity &activity) {
  updateWindowTitle(false);

  strategy->editActivityAtIndex(index, activity);
  activitiesListWidget->updateList();
  slotBoard->groupsList()->updateUI();
}

void MainWindow::saveCurrentActivitiesAsDefault() {}

void MainWindow::saveCurrentStrategyAsDefault() {
  fsIOManager->saveAsDefault(*strategy);
}

void MainWindow::setStrategy(Strategy *newStrategy) {
  strategy = std::unique_ptr<Strategy>(newStrategy);
  updateUI();
}

void MainWindow::updateUI() {
  auto strategyPtr = strategy.get();
  slotBoard->setStrategy(strategyPtr);
  activitiesListWidget->setStrategy(strategyPtr);
  notifier->setStrategy(strategyPtr);
  strategySettingsWidget->setStrategy(strategyPtr);
  updateWindowTitle();
}

bool MainWindow::showAreYouSureDialog(FileSystemIOManager *fsIOManager) {
  QMessageBox msgBox;
  msgBox.setText("Document " + fsIOManager->fileInfo().fileName() +
                 " has been modified.");
  msgBox.setInformativeText("Do you want to save your changes?");
  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                            QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Save);
  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Save:
    fsIOManager->save(*strategy);
    break;
  case QMessageBox::Cancel:
    return false;
  default:
    return true;
  }

  return true;
}

Strategy *MainWindow::openRecentOrNew(bool forceNew) {
  auto lastOpened = fsIOManager->lastOpened();
  if (!forceNew && lastOpened) {
    return new Strategy(lastOpened.value());
  } else {
    fsIOManager->resetFilepath();
    return openDefaultOrNew();
  }
}

Strategy *MainWindow::openDefaultOrNew() {
  if (fsIOManager->defaultStrategyIsSet()) {
    auto defaultStrategy = fsIOManager->defaultStrategy();
    if (defaultStrategy) {
      return new Strategy(defaultStrategy.value());
    }
  }

  return Strategy::createEmpty();
}

void MainWindow::updateRecentFileActions() {
  if (fsIOManager == nullptr || recentMenu == nullptr) {
    return;
  }

  QSettings settings;
  auto files = fsIOManager->recentPaths();

  int numRecentFiles =
      qMin(files.size(), FileSystemIOManager::Settings::numberOfRecent);
  auto recentFileNames = fsIOManager->recentFileNames();

  if (numRecentFiles != 0) {
    for (auto &action : recentMenu->actions()) {
      action->setVisible(true);
    }
  }

  for (int i = 0; i < numRecentFiles; ++i) {
    QString text = recentFileNames[i];
    recentFileActions[i]->setText(text);
    recentFileActions[i]->setData(files[i]);
    recentFileActions[i]->setVisible(true);
  }

  for (int j = numRecentFiles;
       j < FileSystemIOManager::Settings::numberOfRecent; ++j) {
    recentFileActions[j]->setVisible(false);
  }

  if (numRecentFiles == 0) {
    for (auto &action : recentMenu->actions()) {
      action->setVisible(false);
    }
  }
}

void MainWindow::createActions() {
  for (int i = 0; i < FileSystemIOManager::Settings::numberOfRecent; ++i) {
    auto *action = new QAction(this);
    action->setVisible(false);
    connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
    recentFileActions.append(action);
  }
}
