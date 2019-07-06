#ifndef CURRENTACTIVITYWIDGET_H
#define CURRENTACTIVITYWIDGET_H

#include "models/activitygroup.h"
#include "models/strategy.h"
#include <QLabel>
#include <QWidget>
#include <optional>

class CurrentActivityWidget : public QWidget {
  Q_OBJECT
public:
  explicit CurrentActivityWidget(QWidget *parent = nullptr);

  double progress() const;
  void setProgress(double progress);

  void updateUI();

  Strategy *strategy() const;
  void setStrategy(Strategy *strategy);
  void slideAndHide();
  void slideAndShow();

  struct State {
    int startTime;
    int endTime;
    QString activityName;
    QColor activityColor;
    int activityDuration;
    int passedTime;
    int leftTime;

    friend bool operator==(const State &a, const State &b) {
      return a.startTime == b.startTime && a.endTime == b.endTime &&
             a.activityName == b.activityName &&
             a.activityColor == b.activityColor &&
             a.activityDuration == b.activityDuration &&
             a.passedTime == b.passedTime && a.leftTime == b.leftTime;
    }

    friend bool operator!=(const State &a, const State &b) { return !(a == b); }
  };

  State state() const;
  void setState(const State &newState);

signals:
  void clicked();

public slots:
private:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void enterEvent(QEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  State _state;
  double _progress;
  Strategy *_strategy;
  QLabel *activityLabel;
  QLabel *startTimeLabel;
  QLabel *endTimeLabel;
  QLabel *passedTimeLabel;
  QLabel *leftTimeLabel;

  bool isHovered = false;
  bool isClicked = false;
};

#endif // CURRENTACTIVITYWIDGET_H
