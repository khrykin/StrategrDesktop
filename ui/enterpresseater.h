#ifndef ENTERPRESSEATER_H
#define ENTERPRESSEATER_H

#include <QObject>

class EnterPressEater : public QObject {
  Q_OBJECT
public:
  explicit EnterPressEater(QObject *parent = nullptr);

signals:
  void returnKeyPressed();

public slots:

protected:
  bool eventFilter(QObject *obj, QEvent *event);
};

#endif // ENTERPRESSEATER_H
