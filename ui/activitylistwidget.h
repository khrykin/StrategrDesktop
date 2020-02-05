#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QWidget>

#include "strategy.h"
#include "navbar.h"
#include "activityeditormenu.h"
#include "third-party/slidingstackedwidget.h"
#include "activitywidget.h"
#include "reactivelist.hpp"
#include "colorprovider.h"
#include "coloredlabel.h"

class SearchBoxWidget;
class MainScene;
class QVBoxLayout;
class QLineEdit;
class QScrollArea;

class ActivityListWidget : public QWidget,
                           public ReactiveList<ActivityWidget>,
                           public ColorProvider {
Q_OBJECT
public:
    explicit ActivityListWidget(stg::strategy &strategy,
                                QWidget *parent = nullptr);

    void reloadStrategy();
    void showNewActivityMenu();
signals:
    void wantToGetBack();
protected:
    void paintEvent(QPaintEvent *) override;
    bool eventFilter(QObject *object, QEvent *event) override;
private:
    stg::strategy &strategy;
    stg::activity_list searchResults;

    SearchBoxWidget *searchBox = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *listWidget = nullptr;
    Navbar *navbar = nullptr;
    ActivityEditorMenu *newActivityMenu = nullptr;
    ColoredLabel *emptyListNotice = nullptr;

    QAction *getBackAction = nullptr;

    MainScene *mainScene();

    int selectedActivityIndex = -1;

    void layoutChildWidgets();
    void setupNavbar();
    void setupActions();

    void getBack();

    void performSearch();
    bool isShowingSearchResults() const;

    void setSelectedForItemAtIndex(int index, bool isSelected) const;
    void deselectAllItems();

    void updateUI();
    void removeBorderBeforeIndex(int index);

    void selectUp();
    void selectDown();

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, ActivityWidget *itemWidget) override;
    ActivityWidget *makeNewItemAtIndex(int index) override;
    void reconnectItemAtIndex(int itemIndex, ActivityWidget *item);


    void scrollUpItemIntoViewAtIndex(int index);
    void scrollDownItemIntoViewAtIndex(int index);
};

#endif // ACTIVITIESLISTWIDGET_H
