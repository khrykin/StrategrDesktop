#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOption>

#include "activitywidget.h"
#include "applicationsettings.h"
#include "colorutils.h"
#include "mainscene.h"
#include "mainwindow.h"
#include "time_utils.h"
#include "utils.h"

ActivityWidget::ActivityWidget(stg::activity *activity, QWidget *parent)
    : DataProviderWidget(parent), _activity(activity) {
    setFixedHeight(ApplicationSettings::defaultActivityItemHeight);

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(4 * ApplicationSettings::defaultPadding + circleSize, 0,
                                 2 * ApplicationSettings::defaultPadding, 0);
    layout()->setSpacing(0);

    layoutChildWidgets();

    updateUI();
}

void ActivityWidget::layoutChildWidgets() {
    titleLabel = new ColoredLabel();
    titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    titleLabel->setFontHeight(14);
    titleLabel->setElideMode(Qt::ElideMiddle);
    titleLabel->setDynamicColor(textColor);

    durationLabel = new ColoredLabel();
    durationLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    durationLabel->setFontHeight(13);
    durationLabel->setDynamicColor(secondaryTextColor);
    durationLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout()->addWidget(titleLabel);
    layout()->addWidget(durationLabel);
}

void ActivityWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));

    if (isClicked || _isSelected) {
        drawSelection(painter);
    } else if (_drawsBorder) {
        drawBorder(painter);
    }

    auto circleRect = QRect(0, 0, circleSize, circleSize);
    circleRect.moveCenter(QPoint(2 * ApplicationSettings::defaultPadding + circleRect.width() / 2,
                                 contentsRect().center().y()));


    if (activityIsUsed) {
        painter.setBrush(QColor(activity()->color()));
        painter.setPen(Qt::NoPen);
    } else {
        painter.setPen(QPen(QColor(activity()->color()), 1.5));
        painter.setBrush(Qt::NoBrush);
    }

    painter.drawEllipse(circleRect);
}

void ActivityWidget::drawBorder(QPainter &painter) const {
    painter.setBrush(borderColor());
    auto borderRect = QRect(4 * ApplicationSettings::defaultPadding + circleSize,
                            height() - 1,
                            width() - 4 * ApplicationSettings::defaultPadding - circleSize,
                            1);

    painter.drawRect(borderRect);
}

void ActivityWidget::drawSelection(QPainter &painter) const {
    using namespace ColorUtils;

    auto selectionColor = overlayWithAlpha(activity()->color(),
                                           0.05 * shadesAlphaFactor(0));

    auto highlightColor = overlayWithAlpha(activity()->color(),
                                           0.1 * shadesAlphaFactor(0));

    painter.setBrush(_isSelected ? selectionColor : highlightColor);

    auto selectionRect = QRect(0, 0, width(), height());

    painter.drawRect(selectionRect);
}

void ActivityWidget::updateUI() {
    using namespace ColorUtils;
    using namespace stg::time_utils;

    activityIsUsed = strategy().time_slots().has_activity(activity());
    titleLabel->setText(activity()->name().c_str());

    durationLabel->setText(human_string_from_minutes(duration).c_str());
    durationLabel->setDynamicColor(activityIsUsed ? secondaryTextColor : tertiaryTextColor);

    update();
}

stg::activity *ActivityWidget::activity() const {
    return _activity;
}

void ActivityWidget::setActivity(stg::activity *activity) {
    bool newActivityIsUsed = strategy().time_slots().has_activity(activity);
    auto newDuration = strategy().time_slots().duration_for_activity(activity);

    bool activityChanged = activity != _activity;
    bool activityIsUsedChanged = newActivityIsUsed != activityIsUsed;
    bool durationChanged = newDuration != duration;

    _activity = activity;
    activityIsUsed = newActivityIsUsed;
    duration = newDuration;

    if (activityChanged || activityIsUsedChanged || durationChanged)
        updateUI();
}

void ActivityWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() == Qt::LeftButton) {
        isClicked = true;
        update();
        emit hovered();
    }
}

void ActivityWidget::mouseReleaseEvent(QMouseEvent *event) {
    auto wasClicked = isClicked;

    if (wasClicked) {
        choose(event);
    } else {
        emit unhovered();
    }
}

void ActivityWidget::contextMenuEvent(QContextMenuEvent *event) {
    showContextMenu(event->pos());
}

void ActivityWidget::showContextMenu(const QPoint &position) {
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

    connect(editorMenu,
            &ActivityEditorMenu::aboutToHide,
            [=]() {
                emit unhovered();
                isClicked = false;
                update();
            });


    editorMenu->focus();
    editorMenu->exec(mapToGlobal(position));
}

void ActivityWidget::enterEvent(QEvent *event) {
    QWidget::enterEvent(event);
    update();
}

void ActivityWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    update();
}

bool ActivityWidget::drawsBorder() const {
    return _drawsBorder;
}

void ActivityWidget::setDrawsBorder(bool drawsBorder) {
    _drawsBorder = drawsBorder;
    update();
}

bool ActivityWidget::isSelected() const {
    return _isSelected;
}

void ActivityWidget::setIsSelected(bool isSelected) {
    _isSelected = isSelected;
    update();
}

void ActivityWidget::choose(QMouseEvent *event) {
    auto mainScene = qobject_cast<MainWindow *>(window())->scene();

    if (!isClicked) {
        isClicked = true;
        updateUI();
    }

    if (mainScene->selection().empty()) {
        auto pos = event ? event->pos() : contentsRect().center();
        showContextMenu(pos);
    } else {
        emit selected();
        emit unhovered();
        isClicked = false;
        updateUI();
    }
}
