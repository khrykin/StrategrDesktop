#ifndef STRATEGYSETTINGS_H
#define STRATEGYSETTINGS_H

#include "navbar.h"
#include "steppedtimeedit.h"
#include "strategy.h"
#include <QLayout>
#include <QSpinBox>
#include <QWidget>

class MainWindow;
class StrategySettings : public QWidget {
  Q_OBJECT

  Navbar *navBar;
  Strategy *_strategy;
  QVBoxLayout *mainLayout;

  void createLayout();
  void createNavbar();

  void createSlotDurationForm();
  void createStartTimeForm();
  void createEndTimeForm();

  QWidget *makeFormRowWidget();

  void paintEvent(QPaintEvent *) override;
  QLabel *createFormLabel(QString text);

  void updateUI();

  void getBack();
  void save();
  void endTimeChanged(const QTime &time);

  QWidget *_getBackTo = nullptr;

  QSpinBox *slotDurationEdit;
  SteppedTimeEdit *startTimeEdit;
  SteppedTimeEdit *endTimeEdit;

  MainWindow *mainWindow() const;

public:
  explicit StrategySettings(QWidget *parent = nullptr);

  Strategy *getStrategy() const;
  void setStrategy(Strategy *strategy);

  void setGetBackTo(QWidget *value);

  QWidget *getBackTo() const;

  void slideAndHide();
  void slideAndShow();

signals:
  void strategySettingsUpdated();

public slots:
};

#endif // STRATEGYSETTINGS_H
