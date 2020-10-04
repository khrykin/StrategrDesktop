#ifndef STRATEGYSETTINGS_H
#define STRATEGYSETTINGS_H

#include <functional>

#include <QLayout>
#include <QSpinBox>
#include <QWidget>

#include "coloredlabel.h"
#include "dataproviderwidget.h"
#include "navbar.h"
#include "rowwidget.h"
#include "steppedtimeedit.h"
#include "strategy.h"

class StrategySettingsWidget : public DataProviderWidget {
    Q_OBJECT

public:
    explicit StrategySettingsWidget(QWidget *parent = nullptr);

    void reloadStrategy();

    void slideAndHide(const std::function<void()> &onFinishedCallback = nullptr);
    void slideAndShow(const std::function<void()> &onFinishedCallback = nullptr);

private:
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

    QColor labelColor() const;

    void updateUI();

    bool eventFilter(QObject *object, QEvent *event) override;
    void paintEvent(QPaintEvent *) override;
};

#endif// STRATEGYSETTINGS_H
