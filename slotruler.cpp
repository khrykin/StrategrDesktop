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

QVector<QString> SlotRuler::labels() const
{
    return _labels;
}

void SlotRuler::setLabels(QVector<QString> labels)
{
    _labels = labels;
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
    auto indexOfExtraCell = 0; //labels().count();
    if (!!layout()->count()) {
        while (layout()->itemAt(indexOfExtraCell) != nullptr) {
            if (layout()->itemAt(indexOfExtraCell)->widget() != nullptr) {
                layout()->itemAt(indexOfExtraCell)->widget()->hide();
            }
            layout()->removeItem(layout()->itemAt(indexOfExtraCell));
        }
    }


    for (int i = 0; i < labels().count(); i++) {
        QLabel *cell;
        if (layout()->itemAt(i) == nullptr) {
            cell = new QLabel();
            cell->setAlignment(Qt::AlignCenter);
            cell->setFixedHeight(50);
            layout()->addWidget(cell);
        } else {
            cell = dynamic_cast<QLabel *>(layout()->itemAt(i)->widget());
        }

        if (cell != nullptr) {
            cell->setText(labels()[i]);
        }
    }

    auto *mainLayout = static_cast<QVBoxLayout *>(layout());
    mainLayout->addStretch();
}
