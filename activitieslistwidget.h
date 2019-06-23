#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QScrollArea>
#include <QWidget>

#include "navbar.h"
#include "newactivitymenu.h"
#include "slidingstackedwidget.h"
#include "strategy.h"

class ActivitiesListWidget : public QWidget {
  Q_OBJECT
public:
  explicit ActivitiesListWidget(QWidget *parent = nullptr);

  Strategy *strategy() const;
  void setStrategy(Strategy *strategy);
  void updateList();

signals:
  void selectActivity(const Activity &activity);
  void wantNewActivity();
  void activityRemoved(const Activity &activity);
  void activityEditedAtIndex(int index, const Activity &activity);
  void wantToEditActivity(const Activity &activity);
  void activityAppended(const Activity &activity);
public slots:

private:
  void paintEvent(QPaintEvent *) override;

  SlidingStackedWidget *parentStackedWidget();
  void getBack();
  void sendWantNewActivity();

  QScrollArea *scrollArea;
  QWidget *listWidget;
  QWidget *navWidget;
  Strategy *_strategy;
  Navbar *navBar;
  NewActivityMenu *newActivityMenu;
};

#endif // ACTIVITIESLISTWIDGET_H
