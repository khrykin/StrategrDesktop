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

    navWidget = new QWidget();
    navWidget->setLayout(new QHBoxLayout());
    navWidget->layout()->setSpacing(0);
    navWidget->layout()->setMargin(0);
    navWidget->setFixedHeight(50);

    navWidget->setProperty("navWidget", true);
    navWidget->setStyleSheet("[navWidget] {"
                             "border-bottom: 1px solid black;"
                             "");

    layout()->addWidget(navWidget);

    auto *titleLabel = new QLabel("Activities");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px;");

    navWidget->layout()->addWidget(titleLabel);
    layout()->addWidget(navWidget);

    listWidget = new QWidget();
    listWidget->setLayout(new QVBoxLayout());
    listWidget->layout()->setSpacing(0);
    listWidget->layout()->setMargin(0);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(listWidget);

    layout()->addWidget(scrollArea);

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

        listWidget->layout()->addWidget(item);
    }

    listWidget->layout()->addWidget(new QWidget());
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
