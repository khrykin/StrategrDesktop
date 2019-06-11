#include "activitygroupwidget.h"
#include "stacklayout.h"
#include "utils.h"
#include <QDebug>
#include <QLayout>
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

  mainWidget->setLayout(new QHBoxLayout());
  //    mainWidget->layout()->addWidget(label);
  mainWidget->layout()->addWidget(titleLabel);

  selectionWidget = new QWidget(this);
  selectionWidget->hide();

  setLayout(new StackLayout());

  layout()->addWidget(mainWidget);
  layout()->addWidget(selectionWidget);

  updateStyleSheet();
}

void ActivityGroupWidget::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ActivityGroupWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  for (auto selectionIndex : _selectionSlots.keys()) {
    auto *selectionSlot = _selectionSlots[selectionIndex];
    selectionSlot->setGeometry(
        QRect(0, selectionIndex * 50, geometry().width(), 50));
  }
}

void ActivityGroupWidget::updateStyleSheet() {
  if (isSelected()) {
    setStyleSheet("ActivityGroupWidget { "
                  "background-color: #efefef;"
                  "border-bottom: 1px solid #ccc;"
                  "}");
  } else {
    if (activity()) {
      setStyleSheet("ActivityGroupWidget { "
                    "background-color: white;"
                    "border-bottom: 1px solid #ccc;"
                    "}");
    } else {
      setStyleSheet("ActivityGroupWidget { "
                    "background-color: #dedede;"
                    "border-bottom: 1px solid #ccc;"
                    "}");
    }
  }
}

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

void ActivityGroupWidget::setSlotHeight(int height) { setFixedHeight(height); }

void ActivityGroupWidget::updateUI() {
  if (activity().has_value()) {
    auto name = QString::fromStdString(activity()->name);
    auto color = QString::fromStdString(activity()->color);

    titleLabel->setText(name + " " +
                        timeStringForMins(length() * slotDuration()));
    titleLabel->setStyleSheet("color: " + color);
    qDebug() << "titleLabel" << QString::fromStdString(activity()->name)
             << QString::fromStdString(activity()->color);
  } else {
    titleLabel->setText("");
  }

  updateStyleSheet();
}

void ActivityGroupWidget::selectSlotAtIndex(int slotIndex) {
  //    selectionWidget->repaint();
  if (!_selectionSlots.contains(slotIndex)) {
    auto *selectionSlot = new QWidget(selectionWidget);
    selectionSlot->setFixedHeight(50);
    selectionSlot->setStyleSheet("background-color: rgba(255, 255, 0, 0.5)");
    selectionSlot->setGeometry(
        QRect(0, slotIndex * 50, geometry().width(), 50));
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
