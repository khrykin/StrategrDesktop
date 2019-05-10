#include "slotwidget.h"
#include <QPainter>
#include <QStyleOption>
#include <QLayout>

SlotWidget::SlotWidget(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("SlotWidget { "
                  "background-color: white;"
                  "border-bottom: 1px solid gray;"
                  "}");

    label = new QLabel();
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label->setMaximumWidth(30);

    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold;");

    setLayout(new QHBoxLayout());
    layout()->addWidget(label);
    layout()->addWidget(titleLabel);
}

void SlotWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

int SlotWidget::length() const
{
    return _length;
}

void SlotWidget::setLength(int length)
{
    _length = length;
}

void SlotWidget::grow()
{
    setLength(length() + 1);
}

void SlotWidget::shrink()
{
    auto newLength = length() - 1;
    if (newLength < 1) {
        newLength = 1;
    }

    setLength(newLength);
}

QString SlotWidget::title() const
{
    return _title;
}

void SlotWidget::setTitle(QString title)
{
    _title = title;
    titleLabel->setText(title);
}

int SlotWidget::number() const
{
    return _number;
}

void SlotWidget::setNumber(int number)
{
    _number = number;
    label->setText(QString::number(number));
}
