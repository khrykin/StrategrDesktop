#include "activitieslistwidget.h"
#include <QLayout>
#include <QLabel>
#include <QAction>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include "activitieslistitem.h"
#include <QDebug>
#include <QPushButton>

ActivitiesListWidget::ActivitiesListWidget(QWidget *parent) : QWidget(parent)
{
    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);

    auto *navBar = new Navbar();

    layout()->addWidget(navBar);

    navBar->setTitle("Activities");
    navBar->setLeftButton("‹ Back", this, &ActivitiesListWidget::getBack);
    navBar->setRightButton("New", this, &ActivitiesListWidget::sendWantNewActivity);

    listWidget = new QWidget();
    listWidget->setLayout(new QVBoxLayout());
    listWidget->layout()->setSpacing(0);
    listWidget->layout()->setMargin(0);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(listWidget);
    scrollArea->setStyleSheet("QScrollArea {"
                              "border: none;"
                              "}");

    layout()->addWidget(scrollArea);

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
    while (listWidget->layout()->itemAt(0) != nullptr) {
        listWidget->layout()->removeItem(listWidget->layout()->itemAt(0));
    }

    for (unsigned int index = 0; index < strategy()->activities.size(); index++) {
        auto activity = strategy()->activities[index];
        auto *item = new ActivitiesListItem(QString::fromStdString(activity.name));

        connect(item, &ActivitiesListItem::selected, [=] () {
            emit selectActivity(activity);
        });

        connect(item, &ActivitiesListItem::wantToDelete, [=] () {
           this->strategy()->removeActivity(activity);
           item->hide();
           this->listWidget->layout()->removeWidget(item);
           emit activityRemoved(activity);
        });

        listWidget->layout()->addWidget(item);
    }

    listWidget->layout()->addWidget(new QWidget());
}

void ActivitiesListWidget::sendWantNewActivity()
{
    emit wantNewActivity();
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
