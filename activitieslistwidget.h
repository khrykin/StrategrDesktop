#ifndef ACTIVITIESLISTWIDGET_H
#define ACTIVITIESLISTWIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "strategy.h"
#include <QScrollArea>
#include "navbar.h"

class ActivitiesListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActivitiesListWidget(QWidget *parent = nullptr);

    Strategy *strategy() const;
    void setStrategy(Strategy *strategy);

signals:
    void selectActivity(std::shared_ptr<Activity> activity);
    void wantNewActivity();
public slots:

private:
    void paintEvent(QPaintEvent *) override;

    QStackedWidget *parentStackedWidget();
    void getBack();
    void updateList();
    void sendWantNewActivity();

    QScrollArea *scrollArea;
    QWidget *listWidget;
    QWidget *navWidget;
    Strategy *_strategy;
};

#endif // ACTIVITIESLISTWIDGET_H
