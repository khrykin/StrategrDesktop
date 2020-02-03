#ifndef CURRENTACTIVITYWIDGET_H
#define CURRENTACTIVITYWIDGET_H

#include <optional>
#include <functional>

#include <QLabel>
#include <QWidget>

#include "strategy.h"
#include "colorprovider.h"

class ColoredLabel;
class CurrentSessionWidget :
        public QWidget,
        public ColorProvider {
Q_OBJECT
public:
    explicit CurrentSessionWidget(stg::strategy &strategy, QWidget *parent = nullptr);

    double progress() const;
    void setProgress(double progress);

    void reloadStrategy();
    void slideAndHide(const std::function<void()> &onFinishedCallback = nullptr);
    void slideAndShow(const std::function<void()> &onFinishedCallback = nullptr);

    void reloadSessionIfNeeded();

signals:
    void clicked();

public slots:
private:
    // Qt's isVisible() could be glitchy for some reason,
    // so we dont't rely on it and use this flag
    bool isVisible = false;

    double _progress = 0.0;

    stg::strategy &strategy;
    stg::session previousSession;

    ColoredLabel *activityLabel = nullptr;
    ColoredLabel *startTimeLabel = nullptr;
    ColoredLabel *endTimeLabel = nullptr;
    ColoredLabel *passedTimeLabel = nullptr;
    ColoredLabel *leftTimeLabel = nullptr;

    bool isHovered = false;
    bool isClicked = false;

    void updateUIWithSession(const stg::session *session);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    QString makeActivitySessionTitle() const;
};

#endif // CURRENTACTIVITYWIDGET_H
