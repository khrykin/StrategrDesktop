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
#include "rowwidget.h"

class MainWindow;
class StrategySettingsWidget : public QWidget {
Q_OBJECT

public:
    explicit StrategySettingsWidget(stg::strategy &strategy,
                                    QWidget *parent = nullptr);

    void reloadStrategy();

    void slideAndHide(const std::function<void()> &onFinishedCallback = nullptr);
    void slideAndShow(const std::function<void()> &onFinishedCallback = nullptr);

signals:
    void strategySettingsUpdated();

private:
    stg::strategy &strategy;
    bool dontSave = false;

    QVBoxLayout *mainLayout = nullptr;
    QSpinBox *slotDurationEdit = nullptr;
    SteppedTimeEdit *beginTimeEdit = nullptr;
    SteppedTimeEdit *endTimeEdit = nullptr;

    void createLayout();

    void createHeader();
    void createSlotDurationForm();
    void createStartTimeForm();
    void createEndTimeForm();

    RowWidget *makeFormRowWidget();

    static ColoredLabel *makeFormLabel(const QString &text);

    void updateUI();

    void save();
    void endTimeChanged(const QTime &time);

    void paintEvent(QPaintEvent *) override;
    QColor labelColor() const;
};

#endif // STRATEGYSETTINGS_H
