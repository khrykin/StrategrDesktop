#include "activitieslistitem.h"
#include "colorutils.h"
#include <QDebug>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QSpinBox>
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
  label->setStyleSheet("QLineEdit {"
                       "border: none;"
                       "background-color: none;"
                       "font-weight: bold;"
                       "}");

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

  customColorAction = new QAction(tr("Custom Color"));
  customColorAction->setCheckable(true);
  addAction(customColorAction);
  connect(customColorAction, &QAction::triggered, this,
          &ActivitiesListItem::wantToSetCustomColor);

  colorDialog = new QColorDialog(this);
  colorDialog->setOptions(QColorDialog::NoButtons);
  connect(colorDialog, &QColorDialog::currentColorChanged,
          [=](const QColor &color) {
            editActivityColor(color);
            colorPicker->setColor(color);
          });
  connect(colorDialog, &QColorDialog::reject, this,
          &ActivitiesListItem::colorDialogRejected);

  createColorWidgetAction();
  createLineEditWidgetAction();

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

void ActivitiesListItem::wantToSetCustomColor() {
  previousColor = ColorUtils::qColorFromStdString(activity().color);
  colorDialog->setCurrentColor(previousColor);
  colorDialog->close(); // this is a dirty fix, since exec() glitches
  colorDialog->show();
}

void ActivitiesListItem::customColorSet(const QColor &color) {}

void ActivitiesListItem::createLineEditWidgetAction() {
  lineEditWidgetAction = new QWidgetAction(this);
  auto *lineEditWrapper = new QWidget(this);
  auto *lineEditLayout = new QVBoxLayout(lineEditWrapper);
  lineEditLayout->setSpacing(0);
  lineEditWrapper->setLayout(lineEditLayout);
  lineEdit = new QLineEdit(lineEditWrapper);
  lineEdit->setStyleSheet("font-weight: bold;"
                          "font-size: 18pt;"
                          "border: transparent;"
                          "background: transparent;");
  lineEdit->setText(QString::fromStdString(activity().name));
  lineEdit->setFocus();
  lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
  connect(lineEdit, &QLineEdit::editingFinished, [=]() {
    if (contextMenu != nullptr && contextMenu->isVisible()) {
      contextMenu->close();
    }
  });

  lineEditLayout->addWidget(lineEdit);
  lineEditLayout->setContentsMargins(
      lineEditLayout->contentsMargins().left(), 0,
      lineEditLayout->contentsMargins().right(), 0);

  lineEditWidgetAction->setDefaultWidget(lineEditWrapper);
}

void ActivitiesListItem::createColorWidgetAction() {
  colorPicker = new ColorPicker(this);
  colorPicker->setColor(QColor(QString::fromStdString(activity().color)));
  connect(colorPicker, &ColorPicker::colorChanged, this,
          &ActivitiesListItem::colorPickerColorChanged);

  colorWidgetAction = new QWidgetAction(this);
  colorWidgetAction->setDefaultWidget(colorPicker);
}

void ActivitiesListItem::editActivityColor(const QColor &color) {
  auto newActivity = Activity(activity().name, color.name().toStdString());
  emit activityEdited(newActivity);
}

void ActivitiesListItem::editActivityNameFromLineEdit() {
  auto newName = lineEdit->text().toStdString();
  if (newName.empty()) {
    lineEdit->setText(QString::fromStdString(activity().name));
    return;
  }

  if (activity().name == newName) {
    return;
  }

  auto newActivity = Activity(newName, activity().color);
  emit activityEdited(newActivity);
}

void ActivitiesListItem::colorPickerColorChanged(
    const std::optional<QColor> &color) {
  if (!color) {
    return;
  }

  editActivityColor(color.value());

  if (contextMenu) {
    contextMenu->close();
  }
}

void ActivitiesListItem::colorDialogRejected() {
  qDebug() << "Rejected!!!";
  editActivityColor(previousColor);
}

void ActivitiesListItem::updateUI() {
  label->setText(QString::fromStdString(activity().name));
  label->setStyleSheet("font-weight: bold;"
                       "color: " +
                       QString::fromStdString(activity().color));
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
  if (contextMenu != nullptr) {
    //    contextMenu->close();
    delete contextMenu;
  }

  contextMenu = new QMenu(this);
  contextMenu->addAction(lineEditWidgetAction);
  contextMenu->addSeparator();
  contextMenu->addAction(colorWidgetAction);

  customColorAction->setChecked(!colorPicker->color());

  contextMenu->addAction(customColorAction);
  contextMenu->addSeparator();
  contextMenu->addAction(deleteActivityAction);

  connect(contextMenu, &QMenu::aboutToHide,
          [=]() { editActivityNameFromLineEdit(); });

  contextMenu->exec(event->globalPos());

  qDebug() << "contextMenuEvent";
}
