#include "slotruler.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

SlotRuler::SlotRuler(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    setFixedWidth(50);

    setStyleSheet("SlotRuler {"
                  "border-right: 1px solid #aaa;"
                  "}");

    setLayout(layout);
    updateUI();
}

int SlotRuler::numberOfSlots() const
{
    return _numberOfSlots;
}

void SlotRuler::setNumberOfSlots(int numberOfSlots)
{
    _numberOfSlots = numberOfSlots;
    updateUI();
}

void SlotRuler::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SlotRuler::updateUI()
{
    auto indexOfExtraCell = numberOfSlots();
    if (!!layout()->count()) {
        while (layout()->itemAt(indexOfExtraCell) != nullptr) {
            layout()->itemAt(indexOfExtraCell)->widget()->hide();
            layout()->removeWidget(layout()->itemAt(indexOfExtraCell)->widget());
        }
    }


    for (int i = 0; i < numberOfSlots(); i++) {
        QLabel *cell;
        if (layout()->itemAt(i) == nullptr) {
            cell = new QLabel();
            cell->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
            cell->setFixedHeight(50);
            cell->setStyleSheet("border-bottom: 1px solid #000;");

            layout()->addWidget(cell);
        } else {
            cell = static_cast<QLabel *>(layout()->itemAt(i)->widget());
        }

        cell->setText(QString::number(i));
    }
}
