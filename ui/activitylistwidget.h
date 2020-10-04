#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QWidget>

#include "activityeditormenu.h"
#include "activitywidget.h"
#include "coloredlabel.h"
#include "colorprovider.h"
#include "dataproviderwidget.h"
#include "navbar.h"
#include "reactivelist.hpp"
#include "third-party/slidingstackedwidget.h"

class SearchBoxWidget;
class MainScene;
class QVBoxLayout;
class QLineEdit;
class QScrollArea;

class ActivityListWidget : public DataProviderWidget,
                           public ReactiveList<ActivityWidget>,
                           public ColorProvider {
    Q_OBJECT
public:
    explicit ActivityListWidget(QWidget *parent);

    void reloadStrategy();
    void showNewActivityMenu();

protected:
    void paintEvent(QPaintEvent *) override;
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    SearchBoxWidget *searchBox = nullptr;
    QScrollArea *scrollArea = nullptr;
    QWidget *listWidget = nullptr;
    Navbar *navbar = nullptr;
    ActivityEditorMenu *newActivityMenu = nullptr;
    ColoredLabel *emptyListNotice = nullptr;

    QAction *getBackAction = nullptr;

    int selectedActivityIndex = -1;

    void layoutChildWidgets();
    void setupNavbar();
    void setupActions();

    void getBack();

    void performSearch();

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

#endif// ACTIVITIESLISTWIDGET_H
