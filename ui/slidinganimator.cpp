#include <iostream>

#include <QLayout>
#include <QVariant>
#include <QtDebug>

#include "slidinganimator.h"

QVector<QWidget *> SlidingAnimator::widgetsInOperation = QVector<QWidget *>();

void SlidingAnimator::setWidgetSize(int value) {
    (widget->*fixedSizeSetter())(value);
}

int SlidingAnimator::stubSize() {
    return (stub->*sizeGetter<QWidget>())();
}

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
        widget->setGeometry(stub->geometry());
    } else if (operation == Operation::Show) {
        // Enforce initial size on the widget for showAskToSave operation_type.
        // Otherwise it will resize along with the stub, which is not what we want.

        auto geometry = widget->geometry();
        auto origin = makePointByTranslation(geometry.topLeft(), initialWidgetSize);
        auto size = applyInitialSizeToRect(geometry, initialWidgetSize);
        auto rect = QRect(origin, size);

        setWidgetSize(initialWidgetSize);
        widget->setGeometry(rect);
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

SlidingAnimator::Orientation SlidingAnimator::orientation() {
    switch (direction) {
        case Direction::ShowsFromTop:
        case Direction::ShowsFromBottom:
            return Orientation::Vertical;
        case Direction::ShowsFromLeft:
        case Direction::ShowsFromRight:
            return Orientation::Horizontal;
    }
}

QPoint SlidingAnimator::makePointByTranslation(QPoint point, int translation) {
    auto sign = translationSign();
    if (orientation() == Orientation::Vertical) {
        point.setY(point.y() + sign * translation);
    } else {
        point.setX(point.x() + sign * translation);
    }

    return point;
}

int SlidingAnimator::translationSign() {
    switch (direction) {
        case Direction::ShowsFromTop:
        case Direction::ShowsFromLeft: {
            return -1;
        }
        case Direction::ShowsFromBottom:
        case Direction::ShowsFromRight: {
            return 1;
        }
    }
}

QSize SlidingAnimator::applyInitialSizeToRect(QRect rect, int initialSize) {
    if (orientation() == Orientation::Vertical) {
        return QSize(rect.width(), initialSize);
    } else {
        return QSize(initialSize, rect.height());
    }
}

void SlidingAnimator::setWidgetMaximumSize() {
    (widget->*maximumSizeSetter())(widgetSizeHintDimension());
};

int SlidingAnimator::widgetSizeHintDimension() {
    if (orientation() == Orientation::Vertical) {
        return widget->sizeHint().height();
    } else {
        return widget->sizeHint().width();
    }
}

int SlidingAnimator::widgetRelativeCoordinate() {
    return initialWidgetSize - stubSize();
}

int SlidingAnimator::widgetSize() { return (widget->*sizeGetter<QWidget>())(); }

QTimeLine *SlidingAnimator::makeTimeLine() {
    auto *timeLine = new QTimeLine(duration, this);
    timeLine->setUpdateInterval(updateInterval);
    timeLine->setCurveShape(curveShape);
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
    widget->hide();
    teardown();
}

int SlidingAnimator::indexOfWidgetInParentLayout() {
    return widgetParentLayout()->indexOf(widget);
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
    widgetParentLayout()->insertWidget(indexInParentLayout, widget);
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

    stub->layout()->addWidget(widget);
}

void SlidingAnimator::updateWidgetGeometryOnStubResize() {
    if (direction == Direction::ShowsFromBottom || direction == Direction::ShowsFromRight) {
        return;
    }

    auto geometry = widget->geometry();
    auto translation = widgetRelativeCoordinate();
    auto origin = makePointByTranslation(geometry.topLeft(), translation);
    auto rect = QRect(origin, geometry.size());

    widget->setGeometry(rect);
}

void SlidingAnimator::prepareOperation() {
    auto *layout = dynamic_cast<QBoxLayout *>(widget->parentWidget()->layout());

    _widgetParentLayout = layout;

    setWidgetMaximumSize();
    initialWidgetSize = widgetSize();

    createStub();
    commitWidget();

    if (operation == Operation::Show) {
        widget->show();
    }

    timeLine = makeTimeLine();
}

SlidingAnimator::SlidingAnimator(QWidget *widget, const Options &options)
    : QObject(widget),
      widget(widget),
      direction(options.direction),
      duration(options.duration),
      updateInterval(options.updateInterval),
      curveShape(options.curveShape) {}

void SlidingAnimator::show() {
    if (widgetIsInOperation() || !widget->isHidden()) {
        emit done();
        return;
    }

    operation = Operation::Show;
    prepareOperation();

    connect(timeLine, &QTimeLine::valueChanged,
            this, &SlidingAnimator::showFrame);

    connect(timeLine, &QTimeLine::finished,
            this, &SlidingAnimator::showFinished);

    timeLine->start();
}

void SlidingAnimator::commitWidget() {
    widgetsInOperation.append(widget);
}

void SlidingAnimator::decomissionWidget() {
    auto indexOfWidget = widgetsInOperation.indexOf(widget);
    if (indexOfWidget < 0) {
        return;
    }

    widgetsInOperation.removeAt(indexOfWidget);
}

bool SlidingAnimator::widgetIsInOperation() {
    return widgetsInOperation.indexOf(widget) >= 0;
}

void SlidingAnimator::teardown() {
    removeStub();
    decomissionWidget();
    emit done();
}

void SlidingAnimator::hide() {
    if (widgetIsInOperation() || !widget->isVisible()) {
        emit done();
        return;
    }

    operation = Operation::Hide;
    prepareOperation();

    connect(timeLine, &QTimeLine::valueChanged, this, &SlidingAnimator::hideFrame);
    connect(timeLine, &QTimeLine::finished, this, &SlidingAnimator::hideFinished);

    timeLine->start();
}

void SlidingAnimator::hideWidget(QWidget *widget, const Options &options) {
    auto *animator = new SlidingAnimator(widget, options);
    connect(animator, &SlidingAnimator::done, [animator, options]() {
        if (options.onFinishedCallback) {
            options.onFinishedCallback();
        }

        animator->deleteLater();
    });

    animator->hide();
}

void SlidingAnimator::showWidget(QWidget *widget, const Options &options) {
    auto *animator = new SlidingAnimator(widget, options);
    connect(animator, &SlidingAnimator::done, [animator, options]() {
        if (options.onFinishedCallback) {
            options.onFinishedCallback();
        }

        animator->deleteLater();
    });

    animator->show();
}
