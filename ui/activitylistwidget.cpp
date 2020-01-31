#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QScrollBar>

#include "activityinvalidpropertyexception.h"
#include "activitylistwidget.h"
#include "activitywidget.h"
#include "mainscene.h"
#include "slotboardwidget.h"
#include "searchbox.h"

ActivityListWidget::ActivityListWidget(stg::strategy &strategy,
                                       QWidget *parent)
        : strategy(strategy), QWidget(parent) {
    strategy.activities()
            .add_on_change_callback(this, &ActivityListWidget::updateUI);

    setLayout(new QVBoxLayout());
    layout()->setSpacing(0);
    layout()->setContentsMargins(0, 0, 0, 0);

    setupNavbar();
    layoutChildWidgets();
    setupActions();

    auto *action = new QAction("Search Activities...", this);
    action->setShortcut(QKeySequence::Find);
    connect(action,
            &QAction::triggered,
            searchBox,
            &SearchBox::focus);

    addAction(action);

    auto *createActivityAction = new QAction("Create Activity", this);
    createActivityAction->setShortcut(QKeySequence(Qt::Key_N));
    connect(createActivityAction,
            &QAction::triggered,
            this,
            &ActivityListWidget::showNewActivityMenu);

    addAction(createActivityAction);

    scrollArea->installEventFilter(this);
    installEventFilter(this);

    updateUI();
}

void ActivityListWidget::setupActions() {
    getBackAction = new QAction("Back", this);
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
    listWidget->setStyleSheet("[listWidget] {"
                              "background: rgba(255, 255, 255, 0);"
                              "}");

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { "
                              "border: none;"
                              "background: rgba(255, 255, 255, 0);"
                              " }");
    scrollArea->setWidget(listWidget);

    searchBox = new SearchBox("Search activities");
    connect(searchBox,
            &SearchBox::textEdited,
            [=](const QString &text) {
                performSearch();
            });

    connect(searchBox,
            &SearchBox::gotFocus,
            [this]() {
                getBackAction->setEnabled(false);
            });

    connect(searchBox,
            &SearchBox::lostFocus,
            [this]() {
                getBackAction->setEnabled(true);
            });

    auto buttonName = navbar->rightButton() ? "Add" : "+";
    emptyListNotice = new ColoredLabel(QString("Click \"%1\" to add activities").arg(buttonName));
    emptyListNotice->setDynamicColor(&ColorProvider::secondaryTextColor);
    emptyListNotice->setAlignment(Qt::AlignCenter);
    emptyListNotice->setFontHeight(14);

    layout()->addWidget(searchBox);
    layout()->addWidget(emptyListNotice);
    layout()->addWidget(scrollArea);

    newActivityMenu = new ActivityEditorMenu(std::nullopt, this);

    connect(newActivityMenu,
            &ActivityEditorMenu::submitActivity,
            [&](const stg::activity &activity) {
                strategy.add_activity(activity);
                if (isShowingSearchResults()) {
                    performSearch();
                }
            });
}

void ActivityListWidget::setSelectedForItemAtIndex(int index, bool isSelected) const {
    auto listItem = qobject_cast<ActivityWidget *>(
            listWidget->layout()->itemAt(index)->widget());

    if (listItem)
        listItem->setIsSelected(isSelected);
}

void ActivityListWidget::deselectAllItems() {
    for (size_t i = 0; i < strategy.activities().size(); i++) {
        setSelectedForItemAtIndex(i, false);
    }

    selectedActivityIndex = -1;
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
                                              ActivityWidget *item) {
    item->disconnect();

    auto activityIndex = itemIndex;
    if (isShowingSearchResults()) {
        activityIndex = *strategy.activities().index_of(searchResults.at(itemIndex));
    }

    connect(item, &ActivityWidget::selected, [=] {
        strategy.place_activity(activityIndex, mainScene()->selection());
        mainScene()->selection().deselect_all();

        getBack();
    });

    connect(item, &ActivityWidget::activityDeleted, [=] {
        strategy.delete_activity(activityIndex);

        if (isShowingSearchResults()) {
            performSearch();
        }
    });

    connect(item, &ActivityWidget::activityEdited, [=](const stg::activity &newActivity) {
        strategy.edit_activity(activityIndex, newActivity);

        if (isShowingSearchResults()) {
            performSearch();
        }
    });

    connect(item, &ActivityWidget::hovered, [=] {
        deselectAllItems();
        removeBorderBeforeIndex(itemIndex);
    });

    connect(item, &ActivityWidget::unhovered, [=] {
        removeBorderBeforeIndex(0);
    });
}

bool ActivityListWidget::isShowingSearchResults() const {
    return !QRegExp("\\s*").exactMatch(searchBox->text());
}

void ActivityListWidget::performSearch() {
    searchResults = strategy.activities().search(searchBox->text().toStdString());
    updateList();
    deselectAllItems();
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

    newActivityMenu->focus();
    newActivityMenu->exec(center);
}

void ActivityListWidget::reloadStrategy() {
    strategy.activities()
            .add_on_change_callback(this, &ActivityListWidget::updateUI);

    updateUI();
}

int ActivityListWidget::numberOfItems() {
    return isShowingSearchResults()
           ? searchResults.size()
           : strategy.activities().size();
}

QVBoxLayout *ActivityListWidget::listLayout() {
    return qobject_cast<QVBoxLayout *>(listWidget->layout());
}

void ActivityListWidget::reuseItemAtIndex(int index, ActivityWidget *itemWidget) {
    auto activity = isShowingSearchResults()
                    ? searchResults.at(index)
                    : strategy.activities().at(index);
    itemWidget->setActivity(activity);
    reconnectItemAtIndex(index, itemWidget);
}

ActivityWidget *ActivityListWidget::makeNewItemAtIndex(int index) {
    auto activity = isShowingSearchResults()
                    ? searchResults.at(index)
                    : strategy.activities().at(index);

    auto itemWidget = new ActivityWidget(activity);
    reconnectItemAtIndex(index, itemWidget);
    return itemWidget;
}

void ActivityListWidget::removeBorderBeforeIndex(int index) {
    for (auto i = 0; i < listLayout()->count(); i++) {
        if (!listLayout()->itemAt(i)->widget())
            continue;

        auto itemWidget = qobject_cast<ActivityWidget *>(listLayout()->itemAt(i)->widget());

        if (!itemWidget)
            continue;

        itemWidget->setDrawsBorder(i != index - 1);
    }
}

void ActivityListWidget::updateUI() {
    updateList();

    if (strategy.activities().empty()) {
        scrollArea->hide();
        searchBox->hide();
        emptyListNotice->show();
    } else {
        scrollArea->show();
        searchBox->show();
        emptyListNotice->hide();
    }
}

bool ActivityListWidget::eventFilter(QObject *object, QEvent *event) {
    auto keyEvent = dynamic_cast<QKeyEvent *> (event);
    auto isKeyPressEvent = keyEvent &&
                           (keyEvent->type() == QEvent::ShortcutOverride
                            || keyEvent->type() == QEvent::KeyPress);

    if (object == this && isKeyPressEvent) {
//        if (mainScene()->selection().empty())
//            return false;
        if (keyEvent->key() == Qt::Key_Down) {
            selectDown();
            return true;
        } else if (keyEvent->key() == Qt::Key_Up) {
            selectUp();
            return true;
        } else if (keyEvent->key() == Qt::Key_Return ||
                   keyEvent->key() == Qt::Key_Enter) {
            if (selectedActivityIndex >= 0) {
                auto itemWidget = listItemWidgetAtIndex(selectedActivityIndex);
                itemWidget->choose(nullptr);
            }

            return true;
        }
    } else if (object == scrollArea && isKeyPressEvent) {
        return true;
    }

    return false;
}

void ActivityListWidget::selectUp() {
    if (selectedActivityIndex < 0) {
        selectedActivityIndex = -1;
        return;
    }

    auto oldSelectedIndex = selectedActivityIndex;

    deselectAllItems();

    selectedActivityIndex = --oldSelectedIndex;

    if (selectedActivityIndex >= 0 && selectedActivityIndex < numberOfItems()) {
        setSelectedForItemAtIndex(selectedActivityIndex, true);
        scrollUpItemIntoViewAtIndex(selectedActivityIndex);
    }
}

void ActivityListWidget::scrollUpItemIntoViewAtIndex(int index) {
    auto *selectedWidget = listItemWidgetAtIndex(index);
    if (selectedWidget->geometry().top() < scrollArea->verticalScrollBar()->value()) {
        auto newScrollTop = selectedWidget->geometry().top();

        scrollArea->verticalScrollBar()->setValue(newScrollTop);
    }
}

void ActivityListWidget::selectDown() {
    if (selectedActivityIndex >= numberOfItems() - 1) {
        selectedActivityIndex = numberOfItems() - 1;
        return;
    }

    auto oldSelectedIndex = selectedActivityIndex;

    deselectAllItems();

    selectedActivityIndex = ++oldSelectedIndex;

    if (selectedActivityIndex >= 0 && selectedActivityIndex < numberOfItems()) {
        setSelectedForItemAtIndex(selectedActivityIndex, true);

        scrollDownItemIntoViewAtIndex(selectedActivityIndex);
    }
}

void ActivityListWidget::scrollDownItemIntoViewAtIndex(int index) {
    auto *selectedWidget = listItemWidgetAtIndex(selectedActivityIndex);
    if (selectedWidget->geometry().bottom() >
        scrollArea->viewport()->height() + scrollArea->verticalScrollBar()->value()) {
        auto newScrollTop = selectedWidget->geometry().bottom() -
                            scrollArea->viewport()->height();

        scrollArea->verticalScrollBar()->setValue(newScrollTop);
    }
}

