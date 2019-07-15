#ifndef STRATEGYSETTINGS_H
#define STRATEGYSETTINGS_H

#include "models/strategy.h"
#include "navbar.h"
#include "steppedtimeedit.h"
#include <QLayout>
#include <QSpinBox>
#include <QWidget>
#include "parentwindowaccessible.h"

class MainWindow;
class StrategySettingsWidget : public QWidget,
                               public ParentWindowAccessible<StrategySettingsWidget> {
Q_OBJECT

public:
    explicit StrategySettingsWidget(Strategy *strategy,
                                    QWidget *parent = nullptr);

    void setStrategy(Strategy *strategy);

    void slideAndHide();
    void slideAndShow();

    void getBack();
signals:
    void strategySettingsUpdated();

private:
    Strategy *strategy;
    QVBoxLayout *mainLayout;

    bool dontSave = false;

    QSpinBox *slotDurationEdit;
    SteppedTimeEdit *beginTimeEdit;
    SteppedTimeEdit *endTimeEdit;

    void createLayout();

    void createHeader();
    void createSlotDurationForm();
    void createStartTimeForm();
    void createEndTimeForm();

    QWidget *makeFormRowWidget();

    QLabel *makeFormLabel(QString text);

    void updateUI();

    void save();
    void endTimeChanged(const QTime &time);

    void paintEvent(QPaintEvent *) override;
};

#endif // STRATEGYSETTINGS_H
