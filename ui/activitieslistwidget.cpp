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
            .setOnChangeCallback(this, &ActivitiesListWidget::updateUI);

    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setMargin(0);

    setupNavbar();
    layoutChildWidgets();
    setupActions();

    setStyleSheet("ActivitiesListWidget {"
                  "background: white;"
                  "}");

    updateUI();
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
    navbar->setLeftButton("‹ Back",
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

void ActivitiesListWidget::updateUI() {
    auto activitiesCount = strategy->activities().size();

    removeStretch();

    for (int index = 0; index < activitiesCount; index++) {
        reuseItemAtIndex(index);
    }

    removeExtraRows();
    addStretch();
}

void ActivitiesListWidget::addStretch() {
    qobject_cast<QVBoxLayout *>(listWidget->layout())->addStretch();
}

void ActivitiesListWidget::removeExtraRows() {
    auto activitiesCount = strategy->activities().size();
    while (listWidget->layout()->itemAt(activitiesCount) != nullptr) {
        listWidget->layout()->itemAt(activitiesCount)->widget()->hide();
        listWidget->layout()->removeItem(
                listWidget->layout()->itemAt(activitiesCount));
    }
}

void ActivitiesListWidget::reuseItemAtIndex(int index) {
    auto activity = strategy->activities().at(index);
    auto *currentItem = listWidget->layout()->itemAt(index);

    ActivitiesListItem *item;
    if (currentItem) {
        item = qobject_cast<ActivitiesListItem *>(currentItem->widget());
        item->setActivity(activity);
    } else {
        item = new ActivitiesListItem(activity);
        listWidget->layout()->addWidget(item);
    }

    reconnectItemAtIndex(index, item);
}

void ActivitiesListWidget::reconnectItemAtIndex(int index,
                                                ActivitiesListItem *item) {
    item->disconnect();

    connect(item,
            &ActivitiesListItem::selected,
            [=]() {
                strategy->putActivityInTimeSlotsAtIndices(
                        index,
                        mainScene()->selection()
                );

                getBack();
            });

    connect(item,
            &ActivitiesListItem::wantToDelete,
            [=]() {
                strategy->removeActivityAtIndex(index);
                item->hide();
                listWidget->layout()->removeWidget(item);
            });

    connect(item,
            &ActivitiesListItem::activityEdited,
            [=](const Activity &newActivity) {
                strategy->editActivityAtIndex(index, newActivity);
            });
}

void ActivitiesListWidget::removeStretch() {
    for (auto i = 0; i < listWidget->layout()->count(); i++) {
        auto *layoutItem = listWidget->layout()->itemAt(i);
        auto stretch = dynamic_cast<QSpacerItem *>(layoutItem);
        if (stretch) {
            listWidget->layout()->removeItem(stretch);
        }
    }
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
            .setOnChangeCallback(this, &ActivitiesListWidget::updateUI);

    updateUI();
}

void ActivitiesListWidget::addActivity(const Activity &activity) {
    this->strategy->addActivity(activity);
}
