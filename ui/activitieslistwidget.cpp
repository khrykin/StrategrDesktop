#include "activitieslistwidget.h"
#include "activitieslistitem.h"
#include "slotboard.h"
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyleOption>

ActivitiesListWidget::ActivitiesListWidget(QWidget *parent) : QWidget(parent) {
  setLayout(new QVBoxLayout());
  layout()->setSpacing(0);
  layout()->setMargin(0);

  navBar = new Navbar();

  layout()->addWidget(navBar);

  navBar->setTitle("Activities");
  navBar->setLeftButton("‹ Back", this, &ActivitiesListWidget::getBack);
  navBar->setRightButton("New", this,
                         &ActivitiesListWidget::sendWantNewActivity);

  listWidget = new QWidget();
  listWidget->setLayout(new QVBoxLayout());
  listWidget->layout()->setSpacing(0);
  listWidget->layout()->setMargin(0);

  scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(listWidget);
  scrollArea->setStyleSheet("QScrollArea {"
                            "border: none;"
                            "}");

  layout()->addWidget(scrollArea);

  auto *getBackAction = new QAction("Back", this);
  getBackAction->setShortcut(QKeySequence(Qt::Key_Escape));
  addAction(getBackAction);

  connect(getBackAction, &QAction::triggered, this,
          &ActivitiesListWidget::getBack);

  setStyleSheet("ActivitiesListWidget {"
                "background: white;"
                "}");

  newActivityMenu = new NewActivityMenu(this);

  connect(newActivityMenu, &NewActivityMenu::addNewActivity,
          [=](const Activity &activity) { emit activityAppended(activity); });
}

void ActivitiesListWidget::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

SlidingStackedWidget *ActivitiesListWidget::parentStackedWidget() {
  return static_cast<SlidingStackedWidget *>(parentWidget());
}

void ActivitiesListWidget::getBack() {
  parentStackedWidget()->slideToIndex(0);
  auto *slotBoardScrollArea = static_cast<QScrollArea *>(
      parentStackedWidget()->widget(0)->layout()->itemAt(2)->widget());
  auto *slotBoard = static_cast<SlotBoard *>(slotBoardScrollArea->widget());
  slotBoard->groupsList()->deselectAllSlots();
}

void ActivitiesListWidget::updateList() {
  auto activitiesCount = static_cast<int>(strategy()->activities.size());

  for (auto i = 0; i < listWidget->layout()->count(); i++) {
    auto stretch = dynamic_cast<QSpacerItem *>(listWidget->layout()->itemAt(i));
    if (stretch != nullptr) {
      listWidget->layout()->removeItem(stretch);
    }
  }

  for (int index = 0; index < activitiesCount; index++) {
    auto activity = strategy()->activities[static_cast<unsigned int>(index)];

    auto *currentItem = listWidget->layout()->itemAt(index);
    ActivitiesListItem *item;

    if (currentItem == nullptr) {
      item = new ActivitiesListItem(activity);
      listWidget->layout()->addWidget(item);
    } else {
      item = static_cast<ActivitiesListItem *>(currentItem->widget());
      item->setActivity(activity);
    }

    item->disconnect();

    connect(item, &ActivitiesListItem::selected,
            [=]() { emit selectActivity(activity); });

    connect(item, &ActivitiesListItem::wantToDelete, [=]() {
      this->strategy()->removeActivity(activity);
      item->hide();
      this->listWidget->layout()->removeWidget(item);
      emit activityRemoved(activity);
    });

    connect(item, &ActivitiesListItem::wantToEdit,
            [=]() { emit wantToEditActivity(activity); });

    connect(item, &ActivitiesListItem::activityEdited,
            [=](const Activity &activity) {
              emit activityEditedAtIndex(index, activity);
            });
  }

  while (listWidget->layout()->itemAt(activitiesCount) != nullptr) {
    listWidget->layout()->itemAt(activitiesCount)->widget()->hide();
    listWidget->layout()->removeItem(
        listWidget->layout()->itemAt(activitiesCount));
  }

  static_cast<QVBoxLayout *>(listWidget->layout())->addStretch();
}

void ActivitiesListWidget::sendWantNewActivity() {
  auto center =
      QPoint(/*navBar->rightButton()->geometry().center().x()*/
             mapToGlobal(geometry().topRight()).x() -
                 newActivityMenu->sizeHint().width() - 11,
             mapToGlobal(navBar->rightButton()->geometry().bottomLeft()).y() +
                 11);
  newActivityMenu->exec(center);
}

Strategy *ActivitiesListWidget::strategy() const { return _strategy; }

void ActivitiesListWidget::setStrategy(Strategy *strategy) {
  _strategy = strategy;
  updateList();
}
