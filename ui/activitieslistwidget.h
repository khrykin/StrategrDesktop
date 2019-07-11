#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QScrollArea>
#include <QWidget>

#include "models/strategy.h"
#include "navbar.h"
#include "newactivitymenu.h"
#include "third-party/slidingstackedwidget.h"
#include "activitieslistitem.h"

class MainScene;
class ActivitiesListWidget : public QWidget {
Q_OBJECT
public:
    explicit ActivitiesListWidget(Strategy *strategy,
                                  QWidget *parent = nullptr);

    void setStrategy(Strategy *strategy);
signals:
    void wantToGetBack();
    void wantNewActivity();

    void selectActivity(const Activity *activity);
    void activityRemoved(const Activity *activity);
    void activityEditedAtIndex(int index, Activity *activity);
    void wantToEditActivity(const Activity *activity);
    void activityAppended(const Activity *activity);
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

    void addActivity(const Activity &activity);

    void updateUI();
    void paintEvent(QPaintEvent *) override;
    void setupActions();
    void removeStretch();
    void reuseItemAtIndex(int index);
    void removeExtraRows();
    void addStretch();
    void reconnectItemAtIndex(int index, ActivitiesListItem *item);
};

#endif // ACTIVITIESLISTWIDGET_H
