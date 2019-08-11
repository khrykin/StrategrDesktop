#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

#include "activityinvalidpropertyexception.h"
#include "activitylistwidget.h"
#include "activitylistitemwidget.h"
#include "mainscene.h"
#include "slotboard.h"

ActivityListWidget::ActivityListWidget(Strategy *strategy,
                                       QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    strategy->activities()
            .setOnChangeCallback(this, &ActivityListWidget::updateUI);

    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    setupNavbar();
    layoutChildWidgets();
    setupActions();

    updateUI();
}

void ActivityListWidget::setupActions() {
    auto *getBackAction = new QAction("Back", this);
    getBackAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(getBackAction);

    connect(getBackAction, &QAction::triggered, this,
            &ActivityListWidget::getBack);
}

void ActivityListWidget::layoutChildWidgets() {
    listWidget = new QWidget();
    listWidget->setProperty("listWidget", true);
    listWidget->setLayout(new QVBoxLayout());
    listWidget->layout()->setSpacing(0);
    listWidget->layout()->setMargin(0);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { "
                              "border: none;"
                              "background: rgba(255, 255, 255, 0);"
                              " }");
    scrollArea->setWidget(listWidget);
    listWidget->setStyleSheet("[listWidget] {"
                              "background: rgba(255, 255, 255, 0);"
                              "}");

    auto buttonName = navbar->rightButton() ? "Add" : "+";
    emptyListNotice = new ColoredLabel(QString("Click \"%1\" to add activities").arg(buttonName));
    emptyListNotice->setDynamicColor(&ColorProvider::secondaryTextColor);
    emptyListNotice->setAlignment(Qt::AlignCenter);
    emptyListNotice->setFontHeight(14);

    layout()->addWidget(emptyListNotice);
    layout()->addWidget(scrollArea);

    newActivityMenu = new ActivityEditorMenu(std::nullopt, this);

    connect(newActivityMenu,
            &ActivityEditorMenu::submitActivity,
            [&](const Activity &activity) {
                strategy->addActivity(activity);
            });
}

void ActivityListWidget::setupNavbar() {
    navbar = new Navbar();
    layout()->addWidget(navbar);

    navbar->setTitle("Activities");
    navbar->setLeftButton("Back",
                          this,
                          &ActivityListWidget::getBack);
    navbar->setRightButton("Add",
                           this,
                           &ActivityListWidget::showNewActivityMenu);
}

void ActivityListWidget::paintEvent(QPaintEvent *) {
    auto painter = QPainter(this);

    painter.setPen(Qt::NoPen);
    painter.setBrush(baseColor());

    painter.drawRect(QRect(0, 0, width(), height()));
}

MainScene *ActivityListWidget::mainScene() {
    return qobject_cast<MainScene *>(parentWidget());
}

void ActivityListWidget::getBack() {
    emit wantToGetBack();
}

void ActivityListWidget::reconnectItemAtIndex(int itemIndex,
                                              ActivityListItemWidget *item) {
    item->disconnect();

    connect(item, &ActivityListItemWidget::selected,
            [=]() {
                if (mainScene()->selection().empty())
                    return;

                strategy->putActivityInTimeSlotsAtIndices(itemIndex,
                                                          mainScene()->selection());
                getBack();
            });

    connect(item, &ActivityListItemWidget::activityDeleted,
            [=]() {
                strategy->removeActivityAtIndex(itemIndex);
            });

    connect(item, &ActivityListItemWidget::activityEdited,
            [=](const Activity &newActivity) {
                strategy->editActivityAtIndex(itemIndex, newActivity);
            });

    connect(item, &ActivityListItemWidget::hovered, [=]() {
        removeBorderBeforeIndex(itemIndex);
    });

    connect(item, &ActivityListItemWidget::unhovered, [=]() {
        removeBorderBeforeIndex(0);
    });
}

void ActivityListWidget::showNewActivityMenu() {
    const auto margin = 10;

    int topOffset = mapToGlobal(QPoint(0, 0)).y();
    if (navbar->rightButton()) {
        topOffset = mapToGlobal(navbar->rightButton()->geometry().bottomLeft()).y();
    }

    auto center = QPoint(mapToGlobal(geometry().topRight()).x() -
                         newActivityMenu->sizeHint().width() - margin,
                         topOffset + margin);

    newActivityMenu->exec(center);
}

void ActivityListWidget::setStrategy(Strategy *newStrategy) {
    strategy = newStrategy;
    strategy->activities()
            .setOnChangeCallback(this, &ActivityListWidget::updateUI);

    updateUI();
}

int ActivityListWidget::numberOfItems() {
    return strategy->activities().size();
}

QVBoxLayout *ActivityListWidget::listLayout() {
    return qobject_cast<QVBoxLayout *>(listWidget->layout());
}

void ActivityListWidget::reuseItemAtIndex(int index, ActivityListItemWidget *itemWidget) {
    auto activity = strategy->activities().at(index);
    itemWidget->setActivity(activity);
    reconnectItemAtIndex(index, itemWidget);
}

ActivityListItemWidget *ActivityListWidget::makeNewItemAtIndex(int index) {
    auto activity = strategy->activities().at(index);
    auto itemWidget = new ActivityListItemWidget(activity);
    reconnectItemAtIndex(index, itemWidget);
    return itemWidget;
}

void ActivityListWidget::removeBorderBeforeIndex(int index) {
    for (auto i = 0; i < listLayout()->count(); i++) {
        if (!listLayout()->itemAt(i)->widget())
            continue;

        auto itemWidget = qobject_cast<ActivityListItemWidget *>(listLayout()->itemAt(i)->widget());

        if (!itemWidget)
            continue;

        itemWidget->setDrawsBorder(i != index - 1);
    }
}

void ActivityListWidget::updateUI() {
    updateList();

    if (strategy->activities().empty()) {
        scrollArea->hide();
        emptyListNotice->show();
    } else {
        scrollArea->show();
        emptyListNotice->hide();
    }
}
