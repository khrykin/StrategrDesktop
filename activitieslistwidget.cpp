#include "activitieslistwidget.h"
#include <QLayout>
#include <QLabel>
#include <QAction>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include "activitieslistitem.h"
#include <QDebug>

ActivitiesListWidget::ActivitiesListWidget(QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);

//    auto *label = new QLabel("ActivitiesListWidget");
//    label->setAlignment(Qt::AlignCenter);

//    layout()->addWidget(label);

    auto *getBackAction = new QAction("Back", this);
    getBackAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(getBackAction);

    connect(getBackAction,
            &QAction::triggered,
            this,
            &ActivitiesListWidget::getBack);

    setStyleSheet("ActivitiesListWidget {"
                  "background: white;"
                  "}");
}

void ActivitiesListWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}



QStackedWidget *ActivitiesListWidget::parentStackedWidget()
{
    return static_cast<QStackedWidget *>(parentWidget());
}

void ActivitiesListWidget::getBack()
{
    parentStackedWidget()->setCurrentIndex(0);
}

void ActivitiesListWidget::updateList()
{
    for (unsigned int index = 0; index < strategy()->activities.size(); index++) {
        auto activity = strategy()->activities[index];
        auto *item = new ActivitiesListItem(QString::fromStdString(activity->name));
        connect(item, &ActivitiesListItem::selected, [=] () {
            emit selectActivity(activity);
        });

        layout()->addWidget(item);
    }

    layout()->addWidget(new QWidget());
}

Strategy *ActivitiesListWidget::strategy() const
{
    return _strategy;
}

void ActivitiesListWidget::setStrategy(Strategy *strategy)
{
    _strategy = strategy;
    updateList();
}
