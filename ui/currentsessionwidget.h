#ifndef CURRENTACTIVITYWIDGET_H
#define CURRENTACTIVITYWIDGET_H

#include <optional>
#include <functional>

#include <QLabel>
#include <QWidget>
#include <QTimeLine>

#include "strategy.h"
#include "colorprovider.h"

class CurrentSessionWidget : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit CurrentSessionWidget(QWidget *parent = nullptr);

    double progress() const;
    void setProgress(double progress);

    Strategy *strategy() const;
    void setStrategy(Strategy *strategy);
    void slideAndHide(const std::function<void()> &onFinishedCallback = nullptr);
    void slideAndShow(const std::function<void()> &onFinishedCallback = nullptr);

    void setActivitySession(
            const ActivitySession &newActivitySession);

signals:
    void clicked();

public slots:
private:
    double _progress = 0.0;

    ActivitySession activitySession;
    ActivitySession previousSession;

    Strategy *_strategy = nullptr;
    QLabel *activityLabel = nullptr;
    QLabel *startTimeLabel = nullptr;
    QLabel *endTimeLabel = nullptr;
    QLabel *passedTimeLabel = nullptr;
    QLabel *leftTimeLabel = nullptr;

    QTimeLine *paddingAnimator = nullptr;

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
