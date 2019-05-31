#include "slotboard.h"
#include <QHBoxLayout>
#include <QTime>
#include <algorithm>
#include <QDebug>

SlotBoard::SlotBoard(QWidget *parent) : QWidget(parent)
{
    _groupsList = new GroupsList();

    auto *mainLayout = new QHBoxLayout();
    setLayout(mainLayout);
    layout()->setSpacing(0);
    layout()->setMargin(0);



    _slotRuler = new SlotRuler();

    auto *rulerLayout = new QVBoxLayout();
    rulerLayout->setSpacing(0);
    rulerLayout->setMargin(0);
    rulerLayout->addWidget(_slotRuler);

    auto *slotsLayout = new QVBoxLayout();
    slotsLayout->setSpacing(0);
    slotsLayout->setMargin(0);

    auto *header = new QLabel("Start");
    header->setAlignment(Qt::AlignCenter);
    header->setFixedHeight(25);

    auto *footer = new QLabel("End");
    footer->setAlignment(Qt::AlignCenter);
    footer->setFixedHeight(25);

    slotsLayout->addWidget(header);
    slotsLayout->addWidget(_groupsList);
    slotsLayout->addWidget(footer);

    mainLayout->addLayout(rulerLayout);
    mainLayout->addLayout(slotsLayout);
}

Strategy *SlotBoard::strategy() const
{
    return _strategy;
}

void SlotBoard::setStrategy(Strategy *strategy)
{
    _strategy = strategy;
    _groupsList->setStrategy(strategy);

    QVector<QString> labels;

    for (auto &mins : strategy->startTimes()) {
        labels.append(QTime(0, 0, 0)
                      .addSecs(mins * 60)
                      .toString("hh:mm"));
    }

    labels.append(QTime(0, 0, 0)
                  .addSecs(strategy->endTime() * 60)
                  .toString("hh:mm"));

    _slotRuler->setLabels(labels);
}

GroupsList *SlotBoard::groupsList() const
{
    return _groupsList;
}

SlotRuler *SlotBoard::slotRuler() const
{
    return _slotRuler;
}
