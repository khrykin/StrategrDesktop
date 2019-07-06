#ifndef STRATEGYSETTINGS_H
#define STRATEGYSETTINGS_H

#include "models/strategy.h"
#include "navbar.h"
#include "steppedtimeedit.h"
#include <QLayout>
#include <QSpinBox>
#include <QWidget>

class MainWindow;
class StrategySettings : public QWidget {
  Q_OBJECT

public:
  explicit StrategySettings(QWidget *parent = nullptr);

  Strategy *strategy() const;
  void setStrategy(Strategy *strategy);

  void setGetBackTo(QWidget *value);

  QWidget *getBackTo() const;

  void slideAndHide();
  void slideAndShow();

signals:
  void strategySettingsUpdated();

private:
  Navbar *navBar;
  QWidget *header;
  QPushButton *saveButton;
  Strategy *_strategy;
  QVBoxLayout *mainLayout;

  QWidget *_getBackTo = nullptr;

  QSpinBox *slotDurationEdit;
  SteppedTimeEdit *startTimeEdit;
  SteppedTimeEdit *endTimeEdit;

  void createLayout();

  void createHeader();
  void createSlotDurationForm();
  void createStartTimeForm();
  void createEndTimeForm();

  QWidget *makeFormRowWidget();

  QLabel *createFormLabel(QString text);

  void updateUI();

  void getBack();
  void save();
  void endTimeChanged(const QTime &time);

  MainWindow *mainWindow() const;

  void paintEvent(QPaintEvent *) override;
};

#endif // STRATEGYSETTINGS_H
