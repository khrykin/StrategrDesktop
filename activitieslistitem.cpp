#include "activitieslistitem.h"
#include <QDebug>
#include <QLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

ActivitiesListItem::ActivitiesListItem(Activity activity, QWidget *parent)
    : QWidget(parent), _activity(activity) {
  setFixedHeight(50);
  setStyleSheet("ActivitiesListItem {"
                "background-color: white;"
                "border-bottom: 1px solid #ddd;"
                "}"
                "ActivitiesListItem::hover {"
                "background-color: #eee;"
                "border-bottom: 1px solid #ddd;"
                "}");

  setLayout(new QHBoxLayout());
  layout()->setMargin(0);
  layout()->setSpacing(0);

  label = new QLabel();
  label->setAlignment(Qt::AlignCenter);
  label->setStyleSheet("font-weight: bold");

  layout()->addWidget(label);

  deleteActivityAction = new QAction(tr("Delete"), this);
  deleteActivityAction->setShortcut(QKeySequence(Qt::Key_Delete));
  addAction(deleteActivityAction);
  connect(deleteActivityAction, &QAction::triggered, this,
          &ActivitiesListItem::wantDeleteActivity);

  editActivityAction = new QAction(tr("Edit"), this);
  addAction(editActivityAction);
  connect(editActivityAction, &QAction::triggered, this,
          &ActivitiesListItem::wantEditActivity);

  updateUI();
}

void ActivitiesListItem::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ActivitiesListItem::wantDeleteActivity() { emit wantToDelete(); }

void ActivitiesListItem::wantEditActivity() { emit wantToEdit(); }

void ActivitiesListItem::updateUI() {
  label->setText(QString::fromStdString(activity().name));
  label->setStyleSheet("color: " + QString::fromStdString(activity().color));
}

Activity ActivitiesListItem::activity() const { return _activity; }

void ActivitiesListItem::setActivity(const Activity &activity) {
  _activity = activity;
  updateUI();
}

void ActivitiesListItem::mousePressEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::LeftButton) {
    _isClicked = true;
  }

  qDebug() << "mousePressEvent";
}

void ActivitiesListItem::mouseReleaseEvent(QMouseEvent *event) {
  qDebug() << "mouseReleaseEvent";
  if (_isClicked) {
    emit selected();
  }

  _isClicked = false;
}

void ActivitiesListItem::mouseDoubleClickEvent(QMouseEvent *event) {
  qDebug() << "mouseDoubleClickEvent";
}

void ActivitiesListItem::contextMenuEvent(QContextMenuEvent *event) {
  QMenu menu(this);
  menu.addAction(editActivityAction);
  menu.addAction(deleteActivityAction);
  menu.exec(event->globalPos());
  qDebug() << "contextMenuEvent";
}
