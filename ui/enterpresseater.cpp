#include "enterpresseater.h"
#include <QKeyEvent>

EnterPressEater::EnterPressEater(QObject *parent) : QObject(parent) {}

bool EnterPressEater::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Return) {
      emit returnKeyPressed();
      return false;
    }
  }

  // standard event processing
  return QObject::eventFilter(obj, event);
}
