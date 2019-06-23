#include "animatablelayout.h"
#include <QPropertyAnimation>

AnimatableLayout::AnimatableLayout(QWidget *parent)
    : QLayout(parent), item(nullptr) {
  animation = new QPropertyAnimation(this);
  animation->setPropertyName("widgetRect");
  animation->setDuration(400);
  animation->setTargetObject(this);
  mDeltaActive = false;
}

AnimatableLayout::~AnimatableLayout() { delete item; }

QPoint AnimatableLayout::delta() const { return mDelta; }

void AnimatableLayout::setDelta(const QPoint &value) {
  if (mDelta != value) {
    mDelta = value;
    emit deltaChanged(mDelta);
    invalidate();
  }
}

void AnimatableLayout::addItem(QLayoutItem *newItem) {
  Q_ASSERT(!item);
  animation->stop();
  item = newItem;
  emit widgetRectChanged(item->geometry());
  invalidate();
}

QSize AnimatableLayout::sizeHint() const {
  if (!item)
    return QSize();
  QSize result(item->sizeHint());
  result += deltaSize();

  int m = 2 * margin();
  result += QSize(m, m);

  return result;
}

void AnimatableLayout::updateItemPosition() {
  QRect dest = contentsRect();

  QPoint d = delta();
  if (isDeltaActive()) {
    d = -d;
  }

  if (d.x() != 0) {
    if (d.x() > 0) {
      dest.setLeft(dest.left() + d.x());
    } else {
      dest.setRight(dest.right() + d.x());
    }
  }

  if (d.y()) {
    if (d.y() > 0) {
      dest.setTop(dest.top() + d.y());
    } else {
      dest.setBottom(dest.bottom() + d.y());
    }
  }

  animation->setEndValue(dest);
  if (widgetRect() != dest) {
    animation->start();
  }
}

void AnimatableLayout::setGeometry(const QRect &rect) {
  QLayout::setGeometry(rect);

  updateItemPosition();
}

QLayoutItem *AnimatableLayout::itemAt(int i) const { return i == 0 ? item : 0; }

QLayoutItem *AnimatableLayout::takeAt(int i) {
  Q_ASSERT(i == 0);
  QLayoutItem *r = item;
  item = 0;
  return r;
}

void AnimatableLayout::testIt() { setDeltaActive(!isDeltaActive()); }

QRect AnimatableLayout::widgetRect() const {
  if (item)
    return item->geometry();
  return QRect();
}

void AnimatableLayout::setWidgetRect(const QRect &value) {
  if (item && item->geometry() != value) {
    item->setGeometry(value);
    emit widgetRectChanged(item->geometry());
  }
}

bool AnimatableLayout::isDeltaActive() const { return mDeltaActive; }

void AnimatableLayout::setDeltaActive(bool active) {
  if (active != mDeltaActive) {
    mDeltaActive = active;
    animation->stop();
    updateItemPosition();
    emit deltaActiveChanged(active);
  }
}

QSize AnimatableLayout::minimumSize() const {
  QSize result(deltaSize());
  if (item) {
    result += item->minimumSize();
  }
  int m = 2 * margin();
  result += QSize(m, m);
  return result;
}

int AnimatableLayout::count() const { return item ? 1 : 0; }

QSize AnimatableLayout::deltaSize() const {
  return QSize(qAbs(mDelta.x()), qAbs(mDelta.y()));
}
