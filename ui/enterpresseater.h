#ifndef ENTERPRESSEATER_H
#define ENTERPRESSEATER_H

#include <QObject>

class EnterPressEater : public QObject {
    Q_OBJECT
public:
    explicit EnterPressEater(QObject *parent = nullptr);

signals:
    void returnKeyPressed();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif// ENTERPRESSEATER_H
