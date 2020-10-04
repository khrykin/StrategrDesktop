#ifndef CURRENTACTIVITYWIDGET_H
#define CURRENTACTIVITYWIDGET_H

#include <functional>
#include <optional>

#include <QLabel>
#include <QWidget>

#include "colorprovider.h"
#include "dataproviderwidget.h"

class ColoredLabel;
class CurrentSessionWidget : public DataProviderWidget,
                             public ColorProvider {
    Q_OBJECT
public:
    explicit CurrentSessionWidget(QWidget *parent);

    double progress() const;
    void setProgress(double progress);

    void reloadStrategy();
    void slideAndHide(const std::function<void()> &onFinishedCallback = nullptr);
    void slideAndShow(const std::function<void()> &onFinishedCallback = nullptr);

    void reloadSessionIfNeeded();

private:
    double _progress = 0.0;

    std::optional<stg::session> activeSession;

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

    QString makeActivitySessionTitle();
};

#endif// CURRENTACTIVITYWIDGET_H
