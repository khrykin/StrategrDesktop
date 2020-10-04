//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include <QLayout>
#include <QPainter>

#include "drawingutils.h"
#include "selectionwidget.h"

SelectionWidget::SelectionWidget(QWidget *parent) : DataProviderWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);

    selection().add_on_change_callback([this] {
        update();
    });
}


void SelectionWidget::resizeEvent(QResizeEvent *event) {
    //    for (auto *child : children()) {
    //        auto *widget = qobject_cast<QWidget *>(child);
    //        if (widget) {
    //            widget->setFixedWidth(width());
    //        }
    //    }
}

void SelectionWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto penColor = controlColor();
    penColor.setAlphaF(0.5);

    painter.setPen(QPen(penColor, 1));
    //    painter.setPen(Qt::NoPen);

    auto clickedColor = selectionColor();
    clickedColor.setAlphaF(clickedColor.alphaF() * 1.25);

    painter.setBrush(selection().is_clicked() ? clickedColor : selectionColor());

    for (const auto &selectionItem : selection().grouped()) {
        drawSelectionForItem(selectionItem, painter);
    }
}

void SelectionWidget::drawSelectionForItem(const stg::grouped_selection_element &selectionItem,
                                           QPainter &painter) {
    using namespace DrawingUtils;

    auto topPosition = slotHeight() * selectionItem.front() + slotHeight() / 2;
    auto widgetHeight = (int) selectionItem.size() * slotHeight();

    const auto &firstTimeSlot = strategy().time_slots()[selectionItem.front()];
    auto topMargin = firstTimeSlot.begin_time % 60 == 0 ? 4 : 3;

    auto rect = QRect(1,
                      topPosition + topMargin,
                      width() - 2,
                      widgetHeight - topMargin - 2);

    const auto radius = 5;
    const auto roundness = 0.2;

    painter.drawPath(squirclePath(rect, radius, roundness));
}