#ifndef CURRENTTIMEMARKER_H
#define CURRENTTIMEMARKER_H

#include <QWidget>

class CurrentTimeMarkerWidget : public QWidget {
    Q_OBJECT
public:
    explicit CurrentTimeMarkerWidget(QWidget *parent = nullptr);

    static const auto markerSize = 7;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
};

#endif// CURRENTTIMEMARKER_H
