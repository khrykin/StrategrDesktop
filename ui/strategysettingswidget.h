#ifndef STRATEGYSETTINGS_H
#define STRATEGYSETTINGS_H

#include <functional>

#include <QLayout>
#include <QSpinBox>
#include <QWidget>

#include "strategy.h"
#include "navbar.h"
#include "steppedtimeedit.h"
#include "coloredlabel.h"
#include "parentwindowaccessible.h"
#include "rowwidget.h"

class MainWindow;
class StrategySettingsWidget : public QWidget,
                               public ParentWindowAccessible<StrategySettingsWidget> {
Q_OBJECT

public:
    explicit StrategySettingsWidget(Strategy *strategy,
                                    QWidget *parent = nullptr);

    void setStrategy(Strategy *strategy);

    void slideAndHide(const std::function<void()> &onFinishedCallback = nullptr);
    void slideAndShow(const std::function<void()> &onFinishedCallback = nullptr);

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

    RowWidget *makeFormRowWidget();

    ColoredLabel *makeFormLabel(const QString &text);

    void updateUI();

    void save();
    void endTimeChanged(const QTime &time);

    void paintEvent(QPaintEvent *) override;
    QColor labelColor() const;
};

#endif // STRATEGYSETTINGS_H
