#ifndef CURRENTACTIVITYWIDGET_H
#define CURRENTACTIVITYWIDGET_H

#include "strategy.h"
#include <QLabel>
#include <QWidget>
#include <optional>

class CurrentSessionWidget : public QWidget {
Q_OBJECT
public:
    explicit CurrentSessionWidget(QWidget *parent = nullptr);

    double progress() const;
    void setProgress(double progress);

    Strategy *strategy() const;
    void setStrategy(Strategy *strategy);
    void slideAndHide();
    void slideAndShow();

    void setActivitySession(
            const ActivitySession &newActivitySession);
signals:
    void clicked();

public slots:
private:
    double _progress = 0.0;

    ActivitySession activitySession;
    ActivitySession previousSession;

    Strategy *_strategy;
    QLabel *activityLabel;
    QLabel *startTimeLabel;
    QLabel *endTimeLabel;
    QLabel *passedTimeLabel;
    QLabel *leftTimeLabel;

    bool isHovered = false;
    bool isClicked = false;

    void updateUI();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    const QString makeActivitySessionTitle() const;
    double getProgress() const;
};

#endif // CURRENTACTIVITYWIDGET_H
