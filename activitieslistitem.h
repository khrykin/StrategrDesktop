#ifndef ACTIVITIESLISTITEM_H
#define ACTIVITIESLISTITEM_H

#include "activity.h"
#include <QLabel>
#include <QWidget>

class ActivitiesListItem : public QWidget {
  Q_OBJECT
public:
  explicit ActivitiesListItem(Activity activity, QWidget *parent = nullptr);

  Activity activity() const;
  void setActivity(const Activity &activity);

signals:
  void selected();
  void wantToDelete();
  void wantToEdit();

public slots:

private:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void paintEvent(QPaintEvent *) override;

  void wantDeleteActivity();
  void wantEditActivity();

  void updateUI();

  Activity _activity;

  QAction *editActivityAction;
  QAction *deleteActivityAction;

  QLabel *label;
  bool _isClicked = false;
};

#endif // ACTIVITIESLISTITEM_H
