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

class MainScene;
class ActivitiesListWidget : public QWidget,
                             public ReactiveList<ActivitiesListItem> {
Q_OBJECT
public:
    explicit ActivitiesListWidget(Strategy *strategy,
                                  QWidget *parent = nullptr);

    void setStrategy(Strategy *strategy);
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
    void sendWantNewActivity();

    void setupNavbar();
    void layoutChildWidgets();

    void paintEvent(QPaintEvent *) override;
    void setupActions();

    void reconnectItemAtIndex(int index, ActivitiesListItem *item);

    // ReactiveList
    int numberOfItems() override;
    QVBoxLayout *listLayout() override;
    void reuseItemAtIndex(int index, ActivitiesListItem *itemWidget) override;
    ActivitiesListItem *makeNewItemAtIndex(int index) override;
};

#endif // ACTIVITIESLISTWIDGET_H
