#ifndef ACTIVITYEDITOR_H
#define ACTIVITYEDITOR_H

#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QVBoxLayout>

#include "strategy.h"
#include "navbar.h"

class ActivityEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ActivityEditor(QWidget *parent = nullptr);

    std::optional<Activity> &getActivity();
    void setActivity(const std::optional<Activity> &activity);
    void showError(QString key, QString message);
    void reset(std::optional<Activity> activity = std::nullopt);
signals:
    void done(Activity activity);

public slots:

private:
    void getBack();
    void save();

    std::optional<Activity> _activity = std::nullopt;

    void paintEvent(QPaintEvent *) override;

    QStackedWidget *parentStackedWidget();
    QLineEdit *titleEditor;
    QLabel *titleError;
    QVBoxLayout *formLayout;
};

#endif // ACTIVITYEDITOR_H
