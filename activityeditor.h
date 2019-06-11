#ifndef ACTIVITYEDITOR_H
#define ACTIVITYEDITOR_H

#include <QLineEdit>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "navbar.h"
#include "strategy.h"

class ActivityEditor : public QWidget {
  Q_OBJECT
public:
  explicit ActivityEditor(QWidget *parent = nullptr);

  std::optional<Activity> &getActivity();
  void setActivity(const std::optional<Activity> &activity);
  void showError(QString key, QString message);
  void reset(std::optional<Activity> activity = std::nullopt);
signals:
  void done(Activity activity, bool isNew);

public slots:

private:
  void getBack();
  void save();

  std::optional<Activity> _activity = std::nullopt;

  void paintEvent(QPaintEvent *) override;

  QStackedWidget *parentStackedWidget();
  QLineEdit *titleEditor;
  QLabel *titleError;
  QLineEdit *colorEditor;
  QVBoxLayout *formLayout;
  Navbar *navBar;
};

#endif // ACTIVITYEDITOR_H
