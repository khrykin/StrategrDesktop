#include "activitieslistwidget.h"
#include "activitieslistitem.h"
#include "mainscene.h"
#include "slotboard.h"

#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyleOption>

ActivitiesListWidget::ActivitiesListWidget(Strategy *strategy,
                                           QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    strategy->activities()
            .setOnChangeCallback(this, &ActivitiesListWidget::updateList);

    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);

    setupNavbar();
    layoutChildWidgets();
    setupActions();

    setStyleSheet("ActivitiesListWidget {"
                  "background: white;"
                  "}");

    updateList();
}

void ActivitiesListWidget::setupActions() {
    auto *getBackAction = new QAction("Back", this);
    getBackAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(getBackAction);

    connect(getBackAction, &QAction::triggered, this,
            &ActivitiesListWidget::getBack);
}

void ActivitiesListWidget::layoutChildWidgets() {
    listWidget = new QWidget();
    listWidget->setLayout(new QVBoxLayout());
    listWidget->layout()->setSpacing(0);
    listWidget->layout()->setMargin(0);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; }");
    scrollArea->setWidget(listWidget);

    layout()->addWidget(scrollArea);

    newActivityMenu = new NewActivityMenu(this);

    connect(newActivityMenu,
            &NewActivityMenu::addNewActivity,
            [&](const Activity &activity) {
                strategy->addActivity(activity);
            });
}

void ActivitiesListWidget::setupNavbar() {
    navbar = new Navbar();
    layout()->addWidget(navbar);

    navbar->setTitle("Activities");
    navbar->setLeftButton("Back",
                          this,
                          &ActivitiesListWidget::getBack);
    navbar->setRightButton("New",
                           this,
                           &ActivitiesListWidget::sendWantNewActivity);
}

void ActivitiesListWidget::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

MainScene *ActivitiesListWidget::mainScene() {
    return qobject_cast<MainScene *>(parentWidget());
}

void ActivitiesListWidget::getBack() {
    emit wantToGetBack();
}

void ActivitiesListWidget::reconnectItemAtIndex(int index,
                                                ActivitiesListItemWidget *item) {
    item->disconnect();

    connect(item,
            &ActivitiesListItemWidget::selected,
            [=]() {
                strategy->putActivityInTimeSlotsAtIndices(
                        index,
                        mainScene()->selection()
                );

                getBack();
            });

    connect(item,
            &ActivitiesListItemWidget::wantToDelete,
            [=]() {
                strategy->removeActivityAtIndex(index);
            });

    connect(item,
            &ActivitiesListItemWidget::activityEdited,
            [=](const Activity &newActivity) {
                strategy->editActivityAtIndex(index, newActivity);
            });
}

void ActivitiesListWidget::sendWantNewActivity() {
    const auto margin = 10;
    auto center =
            QPoint(mapToGlobal(geometry().topRight()).x() -
                   newActivityMenu->sizeHint().width() - margin,
                   mapToGlobal(navbar->rightButton()->geometry().bottomLeft()).y() +
                   margin);

    newActivityMenu->exec(center);
}

void ActivitiesListWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    strategy->activities()
            .setOnChangeCallback(this, &ActivitiesListWidget::updateList);

    updateList();
}

int ActivitiesListWidget::numberOfItems() {
    return strategy->activities().size();
}

QVBoxLayout *ActivitiesListWidget::listLayout() {
    return qobject_cast<QVBoxLayout *>(listWidget->layout());
}

void ActivitiesListWidget::reuseItemAtIndex(int index, ActivitiesListItemWidget *itemWidget) {
    auto activity = strategy->activities().at(index);
    itemWidget->setActivity(activity);
    reconnectItemAtIndex(index, itemWidget);
}

ActivitiesListItemWidget *ActivitiesListWidget::makeNewItemAtIndex(int index) {
    auto activity = strategy->activities().at(index);
    auto itemWidget = new ActivitiesListItemWidget(activity);
    reconnectItemAtIndex(index, itemWidget);
    return itemWidget;
}
