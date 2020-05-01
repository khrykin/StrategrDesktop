//
// Created by Dmitry Khrykin on 2019-07-11.
//

#include <QLayout>
#include <QPainter>

#include "selectionwidget.h"
#include "colorutils.h"

SelectionWidget::SelectionWidget(stg::strategy &strategy,
                                 int slotHeight,
                                 QWidget *parent)
        : strategy(strategy),
          selection(stg::selection{strategy}),
          slotHeight(slotHeight),
          QWidget(parent) {
    setMouseTracking(false);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    selection.add_on_change_callback([this] {
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

    painter.setBrush(selection.is_clicked()
                     ? clickedColor
                     : selectionColor());

    for (const auto &selectionItem : selection.grouped()) {
        drawSelectionForItem(selectionItem, painter);
    }
}

void SelectionWidget::drawSelectionForItem(const stg::grouped_selection_element &selectionItem,
                                           QPainter &painter) {
    auto topPosition = slotHeight * selectionItem.front();
    auto widgetHeight = static_cast<int>(selectionItem.size()) * slotHeight;

    const auto &lastTimeSlot = strategy.time_slots()[selectionItem.back()];
    auto bottomMargin = lastTimeSlot.end_time() % 60 == 0 ? 1 : 0;

    auto rect = QRect(contentsMargins().left() + 1,
                      contentsMargins().top() + topPosition + 2,
                      width() - contentsMargins().right() - contentsMargins().left() - 1,
                      widgetHeight - bottomMargin - 5);

    painter.drawRoundedRect(rect, 4, 4);
}