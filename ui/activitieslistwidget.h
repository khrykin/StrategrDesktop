#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>

#include "models/strategy.h"
#include "navbar.h"
#include "newactivitymenu.h"
#include "third-party/slidingstackedwidget.h"
#include "activitieslistitem.h"
#include "reactivelist.hpp"
#include "colorprovider.h"

class MainScene;
class ActivitiesListWidget : public QWidget,
                             public ReactiveList<ActivitiesListItemWidget>,
                             public ColorProvider {
Q_OBJECT
public:
    explicit ActivitiesListWidget(Strategy *strategy,
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
    NewActivityMenu *newActivityMenu = nullptr;

    MainScene *mainScene();

    void getBack();

    void setupNavbar();
    void layoutChildWidgets();

    void setupActions();

    void reconnectItemAtIndex(int index, ActivitiesListItemWidget *item);

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, ActivitiesListItemWidget *itemWidget) override;
    ActivitiesListItemWidget *makeNewItemAtIndex(int index) override;

    void paintEvent(QPaintEvent *) override;
};

#endif // ACTIVITIESLISTWIDGET_H
