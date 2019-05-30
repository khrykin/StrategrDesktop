#include "activitygroupwidget.h"
#include <QPainter>
#include <QStyleOption>
#include <QLayout>
#include <QDebug>
#include "stacklayout.h"

ActivityGroupWidget::ActivityGroupWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    mainWidget = new QWidget(this);

//    label = new QLabel();
//    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
//    label->setMaximumWidth(30);

    titleLabel = new QLabel();
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold;");

    mainWidget->setLayout(new QHBoxLayout());
//    mainWidget->layout()->addWidget(label);
    mainWidget->layout()->addWidget(titleLabel);

    selectionWidget = new QWidget(this);
    selectionWidget->hide();

    setLayout(new StackLayout());

    layout()->addWidget(mainWidget);
    layout()->addWidget(selectionWidget);

    updateStyleSheet();
}

void ActivityGroupWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ActivityGroupWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    for (auto selectionIndex : _selectionSlots.keys()) {
        auto *selectionSlot = _selectionSlots[selectionIndex];
        selectionSlot->setGeometry(QRect(0,
                                         selectionIndex * 50,
                                         geometry().width(),
                                         50));
    }
}

void ActivityGroupWidget::updateStyleSheet()
{
    if (title().length()) {
        setStyleSheet("ActivityGroupWidget { "
                      "background-color: white;"
                      "border-bottom: 1px solid #ccc;"
                      "}");
    } else {
        setStyleSheet("ActivityGroupWidget { "
                      "background-color: #dedede;"
                      "border-bottom: 1px solid #ccc;"
                      "}");
    }
}

QMap<int, QWidget *> ActivityGroupWidget::selectionSlots() const
{
    return _selectionSlots;
}

bool ActivityGroupWidget::isSelected() const
{
    return _isSelected;
}

void ActivityGroupWidget::setIsSelected(bool isSelected)
{
    _isSelected = isSelected;

    if (isSelected) {
        setStyleSheet("ActivityGroupWidget { "
                      "background-color: #efefef;"
                      "border-bottom: 1px solid #ccc;"
                      "}");
    } else {
        updateStyleSheet();
    }
}

void ActivityGroupWidget::setSlotHeight(int height)
{
    setFixedHeight(height);
}

void ActivityGroupWidget::selectSlotAtIndex(int slotIndex)
{
//    selectionWidget->repaint();
    if (!_selectionSlots.contains(slotIndex)) {
        auto *selectionSlot = new QWidget(selectionWidget);
        selectionSlot->setFixedHeight(50);
        selectionSlot->setStyleSheet("background-color: rgba(255, 255, 0, 0.5)");
        selectionSlot->setGeometry(QRect(0,
                                         slotIndex * 50,
                                         geometry().width(),
                                         50));
        selectionSlot->show();

        _selectionSlots.insert(slotIndex, selectionSlot);
    } else {
        deselectSlotAtIndex(slotIndex);
    }

    if (selectionWidget->isHidden()) {
        selectionWidget->show();
    }
}

void ActivityGroupWidget::deselectSlotAtIndex(int slotIndex)
{
    if (_selectionSlots[slotIndex] != nullptr) {
        delete _selectionSlots[slotIndex];
    }

    if (_selectionSlots.contains(slotIndex)) {
        _selectionSlots.remove(slotIndex);
    }

    if (!_selectionSlots.count()) {
        selectionWidget->hide();
    }
}

void ActivityGroupWidget::deselectAllSlots()
{
    for (auto selectionIndex : _selectionSlots.keys()) {
        deselectSlotAtIndex(selectionIndex);
    }
    selectionWidget->hide();
}

bool ActivityGroupWidget::hasSelection()
{
    return _selectionSlots.count() > 0;
}

int ActivityGroupWidget::length() const
{
    return _length;
}

void ActivityGroupWidget::setLength(int length)
{
    _length = length;
}

void ActivityGroupWidget::grow()
{
    setLength(length() + 1);
}

void ActivityGroupWidget::shrink()
{
    auto newLength = length() - 1;
    if (newLength < 1) {
        newLength = 1;
    }

    setLength(newLength);
}

QString ActivityGroupWidget::title() const
{
    return _title;
}

void ActivityGroupWidget::setTitle(QString title)
{
    _title = title;
    titleLabel->setText(title);
    updateStyleSheet();
}

int ActivityGroupWidget::number() const
{
    return _number;
}

void ActivityGroupWidget::setNumber(int number)
{
    _number = number;
//    label->setText(QString::number(number));
}
