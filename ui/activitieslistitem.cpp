#include "activitieslistitem.h"
#include "colorutils.h"
#include <QDebug>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOption>
#include <iostream>

#include "applicationsettings.h"

ActivitiesListItemWidget::ActivitiesListItemWidget(Activity *activity,
                                                   QWidget *parent)
        : QWidget(parent), _activity(activity) {
    setFixedHeight(ApplicationSettings::defaultActivityItemHeight);

    setStyleSheet("ActivitiesListItemWidget {"
                  "background-color: white;"
                  "border-bottom: 1px solid #eee;"
                  "}"
                  "ActivitiesListItemWidget::hover {"
                  "background-color: #f6f6f6;"
                  "border-bottom: 1px solid #eee;"
                  "}");

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(10, 0, 10, 0);
    layout()->setSpacing(0);

    layoutChildWidgets();

    setupActions();

    updateUI();
}

void ActivitiesListItemWidget::setupActions() {
    deleteActivityAction = new QAction(tr("Delete"), this);
    deleteActivityAction->setShortcut(QKeySequence(Qt::Key_Delete));
    addAction(deleteActivityAction);
    connect(deleteActivityAction, &QAction::triggered, this,
            &ActivitiesListItemWidget::wantDeleteActivity);

    editActivityAction = new QAction(tr("Edit"), this);
    addAction(editActivityAction);
    connect(editActivityAction, &QAction::triggered, this,
            &ActivitiesListItemWidget::wantEditActivity);

    customColorAction = new QAction(tr("Custom Color"));
    customColorAction->setCheckable(true);
    addAction(customColorAction);
    connect(customColorAction, &QAction::triggered, this,
            &ActivitiesListItemWidget::wantToSetCustomColor);

    colorDialog = new QColorDialog(this);
    colorDialog->setOptions(QColorDialog::NoButtons);
    connect(colorDialog, &QColorDialog::currentColorChanged,
            [=](const QColor &color) {
                editActivityColor(color);
                colorPicker->setColor(color);
            });

    createColorWidgetAction();
    createLineEditWidgetAction();
}

void ActivitiesListItemWidget::layoutChildWidgets() {
    label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLineEdit {"
                         "border: none;"
                         "background-color: none;"
                         "font-weight: bold;"
                         "}");

    layout()->addWidget(label);
}

void ActivitiesListItemWidget::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ActivitiesListItemWidget::wantDeleteActivity() { emit wantToDelete(); }

void ActivitiesListItemWidget::wantEditActivity() { emit wantToEdit(); }

void ActivitiesListItemWidget::wantToSetCustomColor() {
    previousColor = ColorUtils::qColorFromStdString(activity()->color());
    colorDialog->setCurrentColor(previousColor);
    colorDialog->close(); // this is a dirty fix, since exec() glitches
    colorDialog->show();
}

void ActivitiesListItemWidget::createLineEditWidgetAction() {
    lineEditWidgetAction = new QWidgetAction(this);

    auto *lineEditWrapper = new QWidget(this);
    auto *lineEditLayout = new QVBoxLayout(lineEditWrapper);

    lineEditLayout->setSpacing(0);
    lineEditWrapper->setLayout(lineEditLayout);

    lineEdit = new QLineEdit(lineEditWrapper);
    lineEdit->setText(QString::fromStdString(activity()->name()));
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
    lineEdit->setStyleSheet("font-weight: bold;"
                            "font-size: 18pt;"
                            "border: transparent;"
                            "background: transparent;");
}

void ActivitiesListItemWidget::createColorWidgetAction() {
    colorPicker = new ColorPicker(this);
    colorPicker->setColor(QColor(QString::fromStdString(activity()->color())));
    connect(colorPicker, &ColorPicker::colorChanged, this,
            &ActivitiesListItemWidget::colorPickerColorChanged);

    colorWidgetAction = new QWidgetAction(this);
    colorWidgetAction->setDefaultWidget(colorPicker);
}

void ActivitiesListItemWidget::editActivityColor(const QColor &color) {
    auto newColor = color.name().toStdString();
    if (newColor == activity()->color()) {
        return;
    }

    auto newActivity = Activity(activity()->name(), newColor);
    emit activityEdited(newActivity);
}

void ActivitiesListItemWidget::editActivityNameFromLineEdit() {
    auto newName = lineEdit->text().toStdString();
    if (newName.empty()) {
        lineEdit->setText(QString::fromStdString(activity()->name()));
        return;
    }

    if (activity()->name() == newName) {
        return;
    }

    auto newActivity = Activity(newName, activity()->color());
    emit activityEdited(newActivity);
}

void ActivitiesListItemWidget::colorPickerColorChanged(
        const std::optional<QColor> &color) {
    if (!color) {
        return;
    }

    editActivityColor(color.value());

    if (contextMenu) {
        contextMenu->close();
    }
}

void ActivitiesListItemWidget::colorDialogRejected() {
    editActivityColor(previousColor);
}

void ActivitiesListItemWidget::updateUI() {
    label->setText(QString::fromStdString(activity()->name()));
    label->setStyleSheet("font-weight: bold;"
                         "color: " +
                         QString::fromStdString(activity()->color()));
}

Activity *ActivitiesListItemWidget::activity() const { return _activity; }

void ActivitiesListItemWidget::setActivity(Activity *activity) {
    if (activity != _activity) {
        _activity = activity;
        updateUI();
    }
}

void ActivitiesListItemWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        _isClicked = true;
    }
}

void ActivitiesListItemWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (_isClicked) {
        emit selected();
    }

    _isClicked = false;
}

void ActivitiesListItemWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    qDebug() << "mouseDoubleClickEvent";
}

void ActivitiesListItemWidget::contextMenuEvent(QContextMenuEvent *event) {
    delete contextMenu;

    contextMenu = makeContextMenu();

    lineEdit->setText(QString::fromStdString(activity()->name()));
    colorPicker->setColor(ColorUtils::qColorFromStdString(activity()->color()));
    customColorAction->setChecked(!colorPicker->color());

    contextMenu->exec(event->globalPos());
}

QMenu *ActivitiesListItemWidget::makeContextMenu() {
    auto contextMenu = new QMenu(this);
    contextMenu->addAction(lineEditWidgetAction);
    contextMenu->addSeparator();
    contextMenu->addAction(colorWidgetAction);

    contextMenu->addAction(customColorAction);
    contextMenu->addSeparator();
    contextMenu->addAction(deleteActivityAction);

    connect(contextMenu, &QMenu::aboutToHide, [=]() {
        editActivityNameFromLineEdit();
        if (colorPicker->color()) {
            editActivityColor(colorPicker->color().value());
        }
    });

    return contextMenu;
}
