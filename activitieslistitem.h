#ifndef ACTIVITIESLISTITEM_H
#define ACTIVITIESLISTITEM_H

#include "activity.h"
#include "colorpicker.h"
#include "newactivitymenu.h"
#include <QColorDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QWidget>
#include <QWidgetAction>

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
  void activityEdited(const Activity &activity);

public slots:

private:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void paintEvent(QPaintEvent *) override;

  void wantDeleteActivity();
  void wantEditActivity();
  void wantToSetCustomColor();
  void customColorSet(const QColor &color);

  void createLineEditWidgetAction();
  void createColorWidgetAction();

  void editActivityColor(const QColor &color);
  void editActivityNameFromLineEdit();

  void colorPickerColorChanged(const std::optional<QColor> &color);
  void colorDialogRejected();
  void updateUI();

  Activity _activity;

  QAction *editActivityAction;
  QAction *deleteActivityAction;
  QAction *customColorAction;

  QWidgetAction *colorWidgetAction = nullptr;
  QWidgetAction *lineEditWidgetAction = nullptr;

  QMenu *contextMenu = nullptr;
  QColorDialog *colorDialog = nullptr;
  QLineEdit *lineEdit = nullptr;
  QColor previousColor;
  ColorPicker *colorPicker = nullptr;
  QLabel *label;

  bool _isClicked = false;
};

#endif // ACTIVITIESLISTITEM_H
