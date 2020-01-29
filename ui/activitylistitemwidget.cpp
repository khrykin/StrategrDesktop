#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOption>
#include <QPushButton>

#include "activitylistitemwidget.h"
#include "utils.h"
#include "colorutils.h"
#include "applicationsettings.h"

ActivityListItemWidget::ActivityListItemWidget(stg::activity *activity,
                                               QWidget *parent)
        : QWidget(parent), _activity(activity) {
    setFixedHeight(ApplicationSettings::defaultActivityItemHeight);

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(16, 0, 16, 0);
    layout()->setSpacing(0);

    layoutChildWidgets();

    setupActions();

    updateUI();
}

void ActivityListItemWidget::setupActions() {
}

void ActivityListItemWidget::layoutChildWidgets() {
    label = new ColoredLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setBold(true);

    layout()->addWidget(label);
}

void ActivityListItemWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));

    if (isClicked || _isSelected) {
        drawSelection(painter);
    } else if (_drawsBorder) {
        drawBorder(painter);
    }
}

void ActivityListItemWidget::drawBorder(QPainter &painter) const {
    painter.setBrush(borderColor());
    auto borderRect = QRect(8,
                            height() - 1,
                            width() - 2 * ApplicationSettings::defaultPadding,
                            1);

    painter.drawRect(borderRect);
}

void ActivityListItemWidget::drawSelection(QPainter &painter) const {
    painter.setBrush(_isSelected ? selectionColor() : highlightColor());

    auto selectionRect = QRect(0, 0, width(), height());

    painter.drawRect(selectionRect);
}

void ActivityListItemWidget::updateUI() {
    using namespace ColorUtils;
    label->setText(QString::fromStdString(activity()->name()));
    label->setDynamicColor([this]() {
        return safeForegroundColor(QColorFromStdString(activity()->color()));
    });
}

stg::activity *ActivityListItemWidget::activity() const {
    return _activity;
}

void ActivityListItemWidget::setActivity(stg::activity *activity) {
    if (activity != _activity) {
        _activity = activity;
        updateUI();
    }
}

void ActivityListItemWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        isClicked = true;
        update();
        emit hovered();
    }
}

void ActivityListItemWidget::mouseReleaseEvent(QMouseEvent *event) {
    auto wasClicked = isClicked;
    isClicked = false;
    update();

    emit unhovered();

    if (wasClicked)
            emit selected();
}

void ActivityListItemWidget::contextMenuEvent(QContextMenuEvent *event) {
    update();

    auto newEditorMenu = new ActivityEditorMenu(*activity(), this);
    delete editorMenu;
    editorMenu = newEditorMenu;

    connect(editorMenu,
            &ActivityEditorMenu::submitActivity,
            [=](const stg::activity &activity) {
                emit activityEdited(activity);
            });

    connect(editorMenu,
            &ActivityEditorMenu::deleteActivity,
            [=]() {
                emit activityDeleted();
            });

    editorMenu->focus();
    editorMenu->exec(mapToGlobal(event->pos()));
}

void ActivityListItemWidget::enterEvent(QEvent *event) {
    QWidget::enterEvent(event);
    update();
}

void ActivityListItemWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    update();
}

bool ActivityListItemWidget::drawsBorder() const {
    return _drawsBorder;
}

void ActivityListItemWidget::setDrawsBorder(bool drawsBorder) {
    _drawsBorder = drawsBorder;
    update();
}

bool ActivityListItemWidget::isSelected() const {
    return _isSelected;
}

void ActivityListItemWidget::setIsSelected(bool isSelected) {
    _isSelected = isSelected;
    update();
}

void ActivityListItemWidget::choose() {
    emit selected();
}
