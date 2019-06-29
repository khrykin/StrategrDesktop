#include "slidinganimator.h"
#include <QLayout>
#include <QVariant>
#include <QtDebug>

QVector<QWidget *> SlidingAnimator::widgetsInOperation = QVector<QWidget *>();

QWidget *SlidingAnimator::widget() const { return _widget; }

SlidingAnimator::Orientation SlidingAnimator::orientation() const {
  return _orientation;
}

void SlidingAnimator::setOrientation(const Orientation &orientation) {
  _orientation = orientation;
}

void SlidingAnimator::setWidgetSize(int value) {
  (widget()->*fixedSizeSetter())(value);
}

int SlidingAnimator::stubSize() { return (stub->*sizeGetter<QWidget>())(); }

void SlidingAnimator::setStubSize(int value) {
  if (stubSize() != value) {
    (stub->*fixedSizeSetter())(value);
  }
}

void SlidingAnimator::setInitialStubSize() {
  if (operation == Operation::Show) {
    setStubSize(0);
  } else if (operation == Operation::Hide) {
    setStubSize(initialWidgetSize);
  }
}

void SlidingAnimator::setInitialWidgetPosition() {
  if (operation == Operation::Hide) {
    widget()->setGeometry(stub->geometry());
  } else if (operation == Operation::Show) {
    // Enforce initial size on the widget for show operation.
    // Otherwise it will resize along with the stub, which is not what we want.

    auto geometry = widget()->geometry();
    auto origin = makePointBySustraction(geometry.topLeft(), initialWidgetSize);
    auto size = applyInitialSizeToRect(geometry, initialWidgetSize);
    auto rect = QRect(origin, size);

    widget()->setGeometry(rect);
  }
}

SlidingAnimator::WidgetSizeSetterPointer SlidingAnimator::fixedSizeSetter() {
  if (orientation() == Orientation::Vertical) {
    return &QWidget::setFixedHeight;
  } else {
    return &QWidget::setFixedWidth;
  }
}

SlidingAnimator::WidgetSizeSetterPointer SlidingAnimator::maximumSizeSetter() {
  if (orientation() == Orientation::Vertical) {
    return &QWidget::setMaximumHeight;
  } else {
    return &QWidget::setMaximumWidth;
  }
}

SlidingAnimator::WidgetSizeSetterPointer SlidingAnimator::minimumSizeSetter() {
  if (orientation() == Orientation::Vertical) {
    return &QWidget::setMinimumHeight;
  } else {
    return &QWidget::setMinimumWidth;
  }
}

QPoint SlidingAnimator::makePointBySustraction(QPoint point, int translation) {
  if (orientation() == Orientation::Vertical) {
    point.setY(point.y() - translation);
  } else {
    point.setX(point.x() - translation);
  }

  return point;
}

QSize SlidingAnimator::applyInitialSizeToRect(QRect rect, int initialSize) {
  if (orientation() == Orientation::Vertical) {
    return QSize(rect.width(), initialSize);
  } else {
    return QSize(initialSize, rect.height());
  }
}

void SlidingAnimator::setWidgetMaximumSize() {
  (widget()->*maximumSizeSetter())(widgetSizeHintDimension());
};

int SlidingAnimator::widgetSizeHintDimension() {
  if (orientation() == Orientation::Vertical) {
    return widget()->sizeHint().height();
  } else {
    return widget()->sizeHint().width();
  }
}

void SlidingAnimator::resetWidgetMaximumSize() {
  (widget()->*maximumSizeSetter())(QWIDGETSIZE_MAX);
}

int SlidingAnimator::widgetRelativeCoordinate() {
  return initialWidgetSize - stubSize();
}

int SlidingAnimator::widgetSize() {
  return (widget()->*sizeGetter<QWidget>())();
}

QTimeLine *SlidingAnimator::makeTimeLine() {
  QTimeLine *timeLine = new QTimeLine(duration, this);
  timeLine->setUpdateInterval(updateInterval);
  return timeLine;
}

void SlidingAnimator::showFrame(qreal value) {
  auto stubSize = static_cast<int>(initialWidgetSize * value);
  setStubSize(stubSize);
}

void SlidingAnimator::showFinished() {
  setWidgetSize(initialWidgetSize);
  teardown();
}

void SlidingAnimator::hideFrame(qreal value) {
  auto stubSize = static_cast<int>(initialWidgetSize * (1 - value));
  setStubSize(stubSize);
}

void SlidingAnimator::hideFinished() {
  widget()->hide();
  teardown();
}

int SlidingAnimator::indexOfWidgetInParentLayout() {
  return widgetParentLayout()->indexOf(widget());
}

QBoxLayout *SlidingAnimator::widgetParentLayout() {
  return _widgetParentLayout;
}

void SlidingAnimator::createStub() {
  stub = new ResizeAwareWidget();

  placeStubInParentLayout();
  addWidgetToStub();

  setInitialWidgetPosition();
  setInitialStubSize();

  connect(stub, &ResizeAwareWidget::resized, this,
          &SlidingAnimator::updateWidgetGeometryOnStubResize);
}

void SlidingAnimator::removeStub() {
  setWidgetSize(initialWidgetSize);
  widgetParentLayout()->insertWidget(indexInParentLayout, widget());
  widgetParentLayout()->removeWidget(stub);
  delete stub;
}

void SlidingAnimator::placeStubInParentLayout() {
  indexInParentLayout = indexOfWidgetInParentLayout();
  widgetParentLayout()->insertWidget(indexInParentLayout, stub);
}

void SlidingAnimator::addWidgetToStub() {
  auto stubLayout = new QVBoxLayout();
  stubLayout->setContentsMargins(0, 0, 0, 0);
  stubLayout->setSpacing(0);

  stub->setLayout(stubLayout);

  stub->layout()->addWidget(widget());
}

void SlidingAnimator::updateWidgetGeometryOnStubResize() {
  auto geometry = widget()->geometry();
  auto translation = widgetRelativeCoordinate();
  auto origin = makePointBySustraction(geometry.topLeft(), translation);
  auto rect = QRect(origin, geometry.size());

  widget()->setGeometry(rect);
}

void SlidingAnimator::prepareOperation() {
  initialWidgetSize = widgetSize();

  createStub();
  commitWidget();

  if (operation == Operation::Show) {
    widget()->show();
  }

  timeLine = makeTimeLine();
  timeLine->start();
}

SlidingAnimator::SlidingAnimator(QWidget *widget)
    : QObject(widget), _widget(widget) {

  if (widgetIsInOperation()) {
    emit done();
    return;
  }

  _widgetParentLayout =
      static_cast<QBoxLayout *>(widget->parentWidget()->layout());

  setWidgetMaximumSize();
}

SlidingAnimator::~SlidingAnimator() {}

void SlidingAnimator::show() {
  if (widgetIsInOperation()) {
    emit done();
    return;
  }

  operation = Operation::Show;
  prepareOperation();

  connect(timeLine, &QTimeLine::valueChanged, this,
          &SlidingAnimator::showFrame);

  connect(timeLine, &QTimeLine::finished, this, &SlidingAnimator::showFinished);
}

void SlidingAnimator::commitWidget() {
  if (!widgetIsInOperation()) {
    widgetsInOperation.append(widget());
  }
}

void SlidingAnimator::decomissionWidget() {
  auto indexOfWidget = widgetsInOperation.indexOf(widget());
  if (indexOfWidget < 0) {
    return;
  }

  widgetsInOperation.removeAt(indexOfWidget);
}

bool SlidingAnimator::widgetIsInOperation() {
  return widgetsInOperation.indexOf(widget()) >= 0;
}

void SlidingAnimator::teardown() {
  removeStub();
  decomissionWidget();
  emit done();
}

void SlidingAnimator::hide() {
  if (widgetIsInOperation()) {
    emit done();
    return;
  }

  operation = Operation::Hide;
  prepareOperation();

  connect(timeLine, &QTimeLine::valueChanged, this,
          &SlidingAnimator::hideFrame);

  connect(timeLine, &QTimeLine::finished, this, &SlidingAnimator::hideFinished);
}

void SlidingAnimator::hideWidget(QWidget *widget) {
  auto *animator = new SlidingAnimator(widget);
  connect(animator, &SlidingAnimator::done, [animator]() { delete animator; });
  animator->hide();
}

void SlidingAnimator::showWidget(QWidget *widget) {
  auto *animator = new SlidingAnimator(widget);
  connect(animator, &SlidingAnimator::done, [animator]() { delete animator; });
  animator->show();
}
