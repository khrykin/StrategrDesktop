#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOption>
#include <QPushButton>

#include "activitywidget.h"
#include "utils.h"
#include "colorutils.h"
#include "applicationsettings.h"
#include "mainwindow.h"
#include "mainscene.h"

ActivityWidget::ActivityWidget(stg::activity *activity,
                               QWidget *parent)
        : QWidget(parent), _activity(activity) {
    setFixedHeight(ApplicationSettings::defaultActivityItemHeight);

    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(16, 0, 16, 0);
    layout()->setSpacing(0);

    layoutChildWidgets();

    updateUI();
}

void ActivityWidget::layoutChildWidgets() {
    label = new ColoredLabel();
    label->setAlignment(Qt::AlignCenter);
    label->setBold(true);
    label->setFontHeight(ApplicationSettings::sessionFontSize);
    label->setElideMode(Qt::ElideMiddle);

    layout()->addWidget(label);
}

void ActivityWidget::paintEvent(QPaintEvent *) {
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

void ActivityWidget::drawBorder(QPainter &painter) const {
    painter.setBrush(borderColor());
    auto borderRect = QRect(8,
                            height() - 1,
                            width() - 2 * ApplicationSettings::defaultPadding,
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
    label->setText(QString::fromStdString(activity()->name()));
    label->setDynamicColor([this]() {
        return safeForegroundColor(activity()->color());
    });
}

stg::activity *ActivityWidget::activity() const {
    return _activity;
}

void ActivityWidget::setActivity(stg::activity *activity) {
    if (activity != _activity) {
        _activity = activity;
        updateUI();
    }
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
//    isClicked = true;
//    update();

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
        update();
    }

    if (mainScene->selection().empty()) {
        auto pos = event ? event->pos() : contentsRect().center();
        showContextMenu(pos);
    } else {
        emit selected();
        emit unhovered();
        isClicked = false;
        update();
    }
}
