#ifndef STRATEGYSETTINGS_H
#define STRATEGYSETTINGS_H

#include "navbar.h"
#include "steppedtimeedit.h"
#include "strategy.h"
#include <QSpinBox>
#include <QWidget>

class MainWindow;
class StrategySettings : public QWidget {
  Q_OBJECT
public:
  explicit StrategySettings(QWidget *parent = nullptr);

  Strategy *getStrategy() const;
  void setStrategy(Strategy *strategy);

  void setGetBackTo(QWidget *value);

  QWidget *getBackTo() const;

signals:
  void strategySettingsUpdated();

public slots:

private:
  Navbar *navBar;
  Strategy *_strategy;

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
};

#endif // STRATEGYSETTINGS_H
