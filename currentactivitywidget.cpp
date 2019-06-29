#include "currentactivitywidget.h"
#include "slidinganimator.h"
#include "utils.h"
#include <QDebug>
#include <QLayout>
#include <QLocale>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTime>
#include <QTimeLine>

CurrentActivityWidget::CurrentActivityWidget(QWidget *parent)
    : QWidget(parent) {
  setFixedHeight(40);
  setMouseTracking(true);
  auto mainLayout = new QHBoxLayout();
  mainLayout->setContentsMargins(11, 0, 11, 0);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);
  auto *leftLayout = new QVBoxLayout();
  leftLayout->setSpacing(0);
  auto *centerLayout = new QVBoxLayout();
  centerLayout->setSpacing(0);
  auto *rightLayout = new QVBoxLayout();
  rightLayout->setSpacing(0);

  //  setStyleSheet("border: 1px solid red;");
  mainLayout->addLayout(leftLayout);
  mainLayout->addLayout(centerLayout);
  mainLayout->addLayout(rightLayout);

  auto *currentLabel = new QLabel(tr("Current"));
  currentLabel->setStyleSheet("font-weight: bold;"
                              "text-transform: uppercase;"
                              "font-size: 10pt;"
                              "color: #999");
  currentLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

  activityLabel = new QLabel(tr("45 min Guitar"));
  activityLabel->setStyleSheet("font-weight: bold;"
                               "font-size: 15pt;"
                               "color: #888");
  activityLabel->setFixedHeight(25);
  activityLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  startTimeLabel = new QLabel(tr("5:00 PM"));
  startTimeLabel->setStyleSheet("font-weight: bold;"
                                "font-size: 12pt;"
                                "color: #777");
  startTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  endTimeLabel = new QLabel(tr("7:00 PM"));
  endTimeLabel->setStyleSheet("font-weight: bold;"
                              "font-size: 12pt;"
                              "color: #777");
  endTimeLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

  passedTimeLabel = new QLabel(tr("15 min"));
  passedTimeLabel->setStyleSheet("font-weight: bold;"
                                 "font-size: 12pt;"
                                 "color: #999");
  passedTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

  leftTimeLabel = new QLabel(tr("35 min"));
  leftTimeLabel->setStyleSheet("font-weight: bold;"
                               "font-size: 12pt;"
                               "color: #999");
  leftTimeLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);

  leftLayout->addWidget(passedTimeLabel);
  leftLayout->addWidget(startTimeLabel);

  centerLayout->addWidget(currentLabel);
  centerLayout->addWidget(activityLabel);

  rightLayout->addWidget(leftTimeLabel);
  rightLayout->addWidget(endTimeLabel);

  //  mainLayout->addWidget(currentLabel);
  //  mainLayout->addWidget(activityLabel);
  //  mainLayout->addWidget(timeLabel);
  foreach (auto *widget, findChildren<QWidget *>()) {
    widget->setAttribute(Qt::WA_TransparentForMouseEvents);
  };
}

void CurrentActivityWidget::paintEvent(QPaintEvent *event) {
  QPainter painter;
  painter.begin(this);
  auto baseRect = QRect(QPoint(0, 0), geometry().size());
  auto borderRect = QRect(baseRect.bottomLeft().x(), baseRect.bottomLeft().y(),
                          baseRect.width(), 1);
  auto progressRect =
      QRect(baseRect.topLeft().x(), baseRect.topLeft().y(),
            static_cast<int>(baseRect.width() * progress()), baseRect.height());

  auto borderColor = QColor("#ccc");
  auto bgColor = QColor("#F8F8F8");
  auto progressColor = QColor("#ECECEC");
  auto progressBorderColor = borderColor;
  progressBorderColor.setAlpha(0.5 * 255);

  painter.setPen(Qt::NoPen);

  painter.setBrush(bgColor);
  painter.drawRect(baseRect);

  painter.setBrush(progressColor);
  painter.drawRect(progressRect);

  painter.setPen(progressBorderColor);
  painter.drawLine(progressRect.bottomRight(), progressRect.topRight());

  painter.setPen(Qt::NoPen);
  if (isHovered) {
    auto fadeColor = QColor("#000");
    fadeColor.setAlpha(0.05 * 255);
    painter.setBrush(fadeColor);
    painter.drawRect(geometry());
  }

  if (isClicked) {
    auto fadeColor = QColor("#000");
    fadeColor.setAlpha(0.08 * 255);
    painter.setBrush(fadeColor);
    painter.drawRect(geometry());
  }

  painter.setBrush(borderColor);
  painter.drawRect(borderRect);

  painter.end();
}

void CurrentActivityWidget::mousePressEvent(QMouseEvent *) {
  isHovered = false;
  isClicked = true;
  update();
}

void CurrentActivityWidget::enterEvent(QEvent *) {
  isHovered = true;
  update();
}

void CurrentActivityWidget::leaveEvent(QEvent *) {
  isHovered = false;
  update();
}

void CurrentActivityWidget::mouseReleaseEvent(QMouseEvent *) {
  isClicked = false;
  emit clicked();
  update();
}

Strategy *CurrentActivityWidget::strategy() const { return _strategy; }

void CurrentActivityWidget::setStrategy(Strategy *strategy) {
  _strategy = strategy;
  updateUI();
}

void CurrentActivityWidget::slideAndHide() {
  SlidingAnimator::hideWidget(this);
}

void CurrentActivityWidget::slideAndShow() {
  SlidingAnimator::showWidget(this);
}

CurrentActivityWidget::State CurrentActivityWidget::state() const {
  return _state;
}

void CurrentActivityWidget::setState(const State &newState) {
  if (newState == _state) {
    return;
  }

  _state = newState;
  updateUI();
}

void CurrentActivityWidget::updateUI() {
  auto timeFormat = QLocale().timeFormat(QLocale::ShortFormat);
  auto startTimeText =
      QTime(0, 0, 0).addSecs(_state.startTime * 60).toString(timeFormat);
  auto endTimeText =
      QTime(0, 0, 0).addSecs(_state.endTime * 60).toString(timeFormat);

  auto activityText = QString(timeStringForMins(_state.activityDuration) + " " +
                              "<font color=\"" + _state.activityColor.name() +
                              "\">" + _state.activityName + "</font>");

  auto passedTimeText = timeStringForMins(_state.passedTime);
  auto leftTimeText = timeStringForMins(_state.leftTime);

  startTimeLabel->setText(startTimeText);
  endTimeLabel->setText(endTimeText);
  passedTimeLabel->setText(passedTimeText);
  leftTimeLabel->setText(leftTimeText);
  activityLabel->setText(activityText);
}

double CurrentActivityWidget::progress() const { return _progress; }

void CurrentActivityWidget::setProgress(double value) {
  _progress = value;
  update();
}
