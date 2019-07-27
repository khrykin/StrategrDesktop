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

    updateStyleSheet();

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(16, 0, 16, 0);
    layout()->setSpacing(0);

    layoutChildWidgets();

    setupActions();

    updateUI();
}

void ActivitiesListItemWidget::updateStyleSheet() {
    using namespace ColorUtils;

    auto backgroundColor = palette().color(QPalette::Base);
    auto highlightColor = qColorOverlayWithAlpha(
            palette().color(QPalette::Highlight),
            0.5,
            backgroundColor);
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
    label = new ColoredLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setBold(true);

    layout()->addWidget(label);
}

void ActivitiesListItemWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));

    painter.setBrush(borderColor());
    auto borderRect = QRect(16, height() - 1, width() - 2 * 16, 1);
    painter.drawRect(borderRect);

    if (isHovered || isClicked) {
        auto color = isClicked ? highlightColor() : selectionColor();
        painter.setBrush(color);
        auto selectionRect = QRect(16 + 1,
                                   2,
                                   width() - 2 * 16 - 2,
                                   height() - 5);

        painter.drawRoundedRect(selectionRect, 4, 4);
    }
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
    using namespace ColorUtils;
    label->setText(QString::fromStdString(activity()->name()));
    label->setDynamicColor([this]() {
        return safeForegroundColor(qColorFromStdString(activity()->color()));
    });
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
        isClicked = true;
        update();
    }
}

void ActivitiesListItemWidget::mouseReleaseEvent(QMouseEvent *event) {
    isClicked = false;
    update();

    emit selected();
}

void ActivitiesListItemWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    qDebug() << "mouseDoubleClickEvent";
}

void ActivitiesListItemWidget::contextMenuEvent(QContextMenuEvent *event) {
    isHovered = true;
    update();

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

void ActivitiesListItemWidget::enterEvent(QEvent *event) {
    QWidget::enterEvent(event);
    isHovered = true;
    update();
}

void ActivitiesListItemWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    isHovered = false;
    update();
}
