#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>

#include "strategy.h"
#include "navbar.h"
#include "activityeditormenu.h"
#include "third-party/slidingstackedwidget.h"
#include "activitylistitemwidget.h"
#include "reactivelist.hpp"
#include "colorprovider.h"
#include "coloredlabel.h"

class MainScene;
class ActivityListWidget : public QWidget,
                           public ReactiveList<ActivityListItemWidget>,
                           public ColorProvider {
Q_OBJECT
public:
    explicit ActivityListWidget(Strategy *strategy,
                                QWidget *parent = nullptr);

    void setStrategy(Strategy *strategy);
    void showNewActivityMenu();
signals:
    void wantToGetBack();
private:
    QScrollArea *scrollArea = nullptr;
    QWidget *listWidget = nullptr;
    Strategy *strategy = nullptr;
    Navbar *navbar = nullptr;
    ActivityEditorMenu *newActivityMenu = nullptr;
    ColoredLabel *emptyListNotice = nullptr;

    MainScene *mainScene();

    void layoutChildWidgets();
    void setupNavbar();
    void setupActions();

    void getBack();

    void removeBorderBeforeIndex(int index);

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
