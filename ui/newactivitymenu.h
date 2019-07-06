#ifndef NEWACTIVITYMENU_H
#define NEWACTIVITYMENU_H

#include "colorpicker.h"
#include "models/activity.h"
#include <QColorDialog>
#include <QLineEdit>
#include <QMenu>
#include <QWidgetAction>

class NewActivityMenu : public QMenu {
  Q_OBJECT
public:
  explicit NewActivityMenu(QWidget *parent = nullptr);

signals:
  void addNewActivity(const Activity &activity);

public slots:

private:
  void mouseReleaseEvent(QMouseEvent *event) override;
  void createLineEditWidgetAction();
  void createColorWidgetAction();
  void createSaveCancelWidgetAction();
  void wantToSetCustomColor();

  void colorPickerColorChanged(const std::optional<QColor> &color);

  void saveAndClose();
  void cancelAndClose();
  void reset();

  QWidgetAction *lineEditWidgetAction;
  QWidgetAction *colorWidgetAction;
  QWidgetAction *saveCancelWidgetAction;

  QAction *saveAction;
  QAction *customColorAction;
  QLineEdit *lineEdit;
  QColorDialog *colorDialog;
  ColorPicker *colorPicker;
  QColor currentColor;
};

#endif // NEWACTIVITYMENU_H
