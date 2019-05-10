#include "slotwidget.h"
#include <QPainter>
#include <QStyleOption>
#include <QLayout>

SlotWidget::SlotWidget(QWidget *parent) : QWidget(parent)
{
    label = new QLabel();
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    label->setMaximumWidth(30);

    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold;");

    setLayout(new QHBoxLayout());
    layout()->addWidget(label);
    layout()->addWidget(titleLabel);

    updateStyleSheet();
}

void SlotWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SlotWidget::updateStyleSheet()
{
    if (title().length()) {
        setStyleSheet("SlotWidget { "
                      "background-color: white;"
                      "border-bottom: 1px solid gray;"
                      "}");
    } else {
        setStyleSheet("SlotWidget { "
                      "background-color: #ccc;"
                      "border-bottom: 1px solid gray;"
                      "}");
    }
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
    updateStyleSheet();
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
