#include "slotboard.h"
#include <QHBoxLayout>

SlotBoard::SlotBoard(QWidget *parent) : QWidget(parent)
{
    _groupsList = new GroupsList();
    setLayout(new QHBoxLayout);
    layout()->setSpacing(0);
    layout()->setMargin(0);

    _slotRuler = new SlotRuler();
    layout()->addWidget(_slotRuler);
    layout()->addWidget(_groupsList);
}

Strategy *SlotBoard::strategy() const
{
    return _strategy;
}

void SlotBoard::setStrategy(Strategy *strategy)
{
    _strategy = strategy;
    _groupsList->setStrategy(strategy);
    _slotRuler->setNumberOfSlots(strategy->numberOfSlots);
}

GroupsList *SlotBoard::groupsList() const
{
    return _groupsList;
}

SlotRuler *SlotBoard::slotRuler() const
{
    return _slotRuler;
}
