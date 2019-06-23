#include "activitygroupwidget.h"
#include "stacklayout.h"
#include "utils.h"
#include <QDebug>
#include <QLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>

ActivityGroupWidget::ActivityGroupWidget(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_TransparentForMouseEvents);
  mainWidget = new QWidget(this);

  //    label = new QLabel();
  //    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  //    label->setMaximumWidth(30);

  titleLabel = new QLabel();
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet("font-weight: bold;");

  auto *mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  mainWidget->setLayout(mainLayout);
  //    mainWidget->layout()->addWidget(label);
  mainWidget->layout()->addWidget(titleLabel);

  selectionWidget = new QWidget(this);
  selectionWidget->hide();

  setLayout(new StackLayout());

  layout()->addWidget(mainWidget);
  layout()->addWidget(selectionWidget);

  updateStyleSheet();
}

void ActivityGroupWidget::paintEvent(QPaintEvent *event) {
  QStyleOption opt;
  opt.init(this);
  QPainter painter(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

  //  // Workaround for double rendering under scrollbar
  //  if (event->rect().width() < 30) {
  //    return;
  //  }

  //  auto height =
  //      event->rect().height() > slotHeight ? event->rect().height() :
  //      slotHeight;
  //  auto y = event->rect().y() < geometry().height() - slotHeight
  //               ? event->rect().y()
  //               : geometry().height() - slotHeight;
  //  auto textRect = QRect(0, y, geometry().width(), height);

  //  painter.drawText(textRect, Qt::AlignCenter,
  //                   activity() ? QString::fromStdString(activity()->name) :
  //                   "");
}

void ActivityGroupWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  for (auto selectionIndex : _selectionSlots.keys()) {
    auto *selectionSlot = _selectionSlots[selectionIndex];
    selectionSlot->setGeometry(
        QRect(0, selectionIndex * slotHeight, geometry().width(), slotHeight));
  }

  update();
}

void ActivityGroupWidget::updateStyleSheet() {
  auto leftBorderStyle =
      activity() ? "border-left: 3px solid " +
                       QString::fromStdString(activity()->color) + ";"
                 : "";

  if (isSelected()) {
    setStyleSheet("ActivityGroupWidget { "
                  "background-color: #efefef;" +
                  leftBorderStyle +
                  "border-bottom: 1px solid #d8d8d8;"
                  "}");
  } else {
    if (activity()) {
      setStyleSheet("ActivityGroupWidget { "
                    "background-color: white;" +
                    leftBorderStyle +
                    "border-bottom: 1px solid #d8d8d8;"
                    "}");
    } else {
      setStyleSheet("ActivityGroupWidget { "
                    "background-color: #f4f4f4;"
                    "border-bottom: 1px solid #d8d8d8;"
                    "}");
    }
  }
}

void ActivityGroupWidget::setSlotHeight(int value) { slotHeight = value; }

int ActivityGroupWidget::slotDuration() const { return _slotDuration; }

void ActivityGroupWidget::setSlotDuration(int slotDuration) {
  _slotDuration = slotDuration;
  updateUI();
}

QMap<int, QWidget *> ActivityGroupWidget::selectionSlots() const {
  return _selectionSlots;
}

bool ActivityGroupWidget::isSelected() const { return _isSelected; }

void ActivityGroupWidget::setIsSelected(bool isSelected) {
  _isSelected = isSelected;

  updateStyleSheet();
}

void ActivityGroupWidget::setHeight(int height) { setFixedHeight(height); }

void ActivityGroupWidget::updateUI() {
  if (activity()) {
    auto name = QString::fromStdString(activity()->name);
    auto color = QString::fromStdString(activity()->color);

    titleLabel->setText("<font color=\"#888\"><b>" +
                        timeStringForMins(length() * slotDuration()) +
                        "</b></font> <font color=\"" + color + "\"><b>" + name +
                        "</b></font>");

    titleLabel->setStyleSheet("font-size: 15pt;");

  } else {
    titleLabel->setText("");
  }

  update();
  updateStyleSheet();
}

void ActivityGroupWidget::selectSlotAtIndex(int slotIndex) {
  //    selectionWidget->repaint();
  auto color = QColor("#BDD6F1");
  auto bgColorStyle = QString("background-color: rgba(%1, %2, %3, 0.5);")
                          .arg(color.red())
                          .arg(color.green())
                          .arg(color.blue());
  auto borderTopStyle = QString("border-top: 1px solid #007AFF");
  auto borderBottomStyle = QString("border-bottom: 1px solid #007AFF");

  if (!_selectionSlots.contains(slotIndex)) {
    auto *selectionSlot = new QWidget(selectionWidget);
    selectionSlot->setFixedHeight(slotHeight);

    selectionSlot->setStyleSheet(bgColorStyle);

    selectionSlot->setGeometry(
        QRect(0, slotIndex * slotHeight, geometry().width(), slotHeight));
    selectionSlot->show();

    _selectionSlots.insert(slotIndex, selectionSlot);
  } else {
    deselectSlotAtIndex(slotIndex);
  }

  if (selectionWidget->isHidden()) {
    selectionWidget->show();
  }
}

void ActivityGroupWidget::deselectSlotAtIndex(int slotIndex) {
  if (_selectionSlots[slotIndex] != nullptr) {
    delete _selectionSlots[slotIndex];
  }

  if (_selectionSlots.contains(slotIndex)) {
    _selectionSlots.remove(slotIndex);
  }

  if (!_selectionSlots.count()) {
    selectionWidget->hide();
  }
}

void ActivityGroupWidget::deselectAllSlots() {
  for (auto selectionIndex : _selectionSlots.keys()) {
    deselectSlotAtIndex(selectionIndex);
  }
  selectionWidget->hide();
}

void ActivityGroupWidget::selectAllSlots() {
  for (auto selectionIndex = 0; selectionIndex < length(); selectionIndex++) {
    selectSlotAtIndex(selectionIndex);
  }

  if (selectionWidget->isHidden()) {
    selectionWidget->show();
  }
}

bool ActivityGroupWidget::hasSelection() { return _selectionSlots.count() > 0; }

int ActivityGroupWidget::length() const { return _length; }

void ActivityGroupWidget::setLength(unsigned int length) {
  _length = static_cast<int>(length);
  updateUI();
}

std::optional<Activity> ActivityGroupWidget::activity() const {
  return _activity;
}

void ActivityGroupWidget::setActivity(std::optional<Activity> activity) {
  _activity = activity;
  updateUI();
}

int ActivityGroupWidget::number() const { return _number; }

void ActivityGroupWidget::setNumber(int number) {
  _number = number;
  //    label->setText(QString::number(number));
}
