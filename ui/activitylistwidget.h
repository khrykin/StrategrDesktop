#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>

#include "Strategy.h"
#include "navbar.h"
#include "activityeditormenu.h"
#include "third-party/slidingstackedwidget.h"
#include "activitylistitemwidget.h"
#include "reactivelist.hpp"
#include "colorprovider.h"
#include "coloredlabel.h"

class SearchBox;
class MainScene;

class ActivityListWidget : public QWidget,
                           public ReactiveList<ActivityListItemWidget>,
                           public ColorProvider {
Q_OBJECT
public:
    explicit ActivityListWidget(Strategy &strategy,
                                QWidget *parent = nullptr);

    void reloadStrategy();
    void showNewActivityMenu();
signals:
    void wantToGetBack();
private:
    Strategy &strategy;
    ActivityList searchResults;

    SearchBox *searchBox = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *listWidget = nullptr;
    Navbar *navbar = nullptr;
    ActivityEditorMenu *newActivityMenu = nullptr;
    ColoredLabel *emptyListNotice = nullptr;

    MainScene *mainScene();

    void layoutChildWidgets();
    void setupNavbar();
    void setupActions();

    void getBack();

    void removeBorderBeforeIndex(int index);

    void performSearch();
    bool isSearching() const;

    void updateUI();

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, ActivityListItemWidget *itemWidget) override;
    ActivityListItemWidget *makeNewItemAtIndex(int index) override;
    void reconnectItemAtIndex(int itemIndex, ActivityListItemWidget *item);

    void paintEvent(QPaintEvent *) override;

};

#endif // ACTIVITIESLISTWIDGET_H
