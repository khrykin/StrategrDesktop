#ifndef SLIDINGANIMATOR_H
#define SLIDINGANIMATOR_H

#include <QLayout>
#include <QTimeLine>
#include <QWidget>

#include <functional>

class SlidingAnimator : public QObject {
    Q_OBJECT
private:
    static const auto defaultDuration = 150;
    static const auto defaultUpdateInterval = 20;
    static const auto defaultCurveShape = QTimeLine::EaseInOutCurve;

public:
    enum class Direction {
        ShowsFromTop,
        ShowsFromBottom,
        ShowsFromLeft,
        ShowsFromRight
    };

    struct Options {
        Direction direction = Direction::ShowsFromTop;
        int duration = defaultDuration;
        int updateInterval = defaultUpdateInterval;
        QTimeLine::CurveShape curveShape = defaultCurveShape;
        std::function<void()> onFinishedCallback = nullptr;

        // Explicit constructor definition is needed here because of clang bug.
        // See: https://stackoverflow.com/questions/53408962
        Options(){};
    };

    static void hideWidget(QWidget *widget, const Options &options = Options());
    static void showWidget(QWidget *widget, const Options &options = Options());

private:
    class ResizeAwareWidget;

    enum class Operation {
        Show,
        Hide
    };

    enum class Orientation {
        Horizontal,
        Vertical
    };

    using WidgetSizeSetterPointer = void (QWidget::*)(int);
    template<class T>
    using SizeGetterPointer = int (T::*)() const;

    static QVector<QWidget *> widgetsInOperation;

    QWidget *widget;
    QBoxLayout *_widgetParentLayout;
    QTimeLine *timeLine;

    Direction direction;
    int duration;
    int updateInterval;
    QTimeLine::CurveShape curveShape;

    Operation operation;

    int initialWidgetMinimumSize;
    int initialWidgetMaximumSize;

    int initialWidgetSize;
    int indexInParentLayout;

    explicit SlidingAnimator(QWidget *widget = nullptr, const Options &options = Options());

    WidgetSizeSetterPointer fixedSizeSetter();
    WidgetSizeSetterPointer maximumSizeSetter();

    template<class T>
    SizeGetterPointer<T> sizeGetter() {
        if (orientation() == Orientation::Vertical) {
            return &T::height;
        } else {
            return &T::width;
        }
    }

    Orientation orientation();

    QPoint makePointByTranslation(QPoint point, int translation);
    int translationSign();

    QSize applyInitialSizeToRect(QRect rect, int initialSize);

    void setWidgetMaximumSize();

    int widgetRelativeCoordinate();

    int widgetSize();
    void setWidgetSize(int value);

    int stubSize();
    void setStubSize(int value);

    void setInitialStubSize();
    void setInitialWidgetPosition();

    int widgetSizeHintDimension();

    QTimeLine *makeTimeLine();

    void showFrame(qreal value);
    void showFinished();

    void hideFrame(qreal value);
    void hideFinished();

    int indexOfWidgetInParentLayout();

    QBoxLayout *widgetParentLayout();

    void createStub();
    void removeStub();

    void placeStubInParentLayout();
    void addWidgetToStub();

    ResizeAwareWidget *stub;

    void updateWidgetGeometryOnStubResize();
    void prepareOperation();

    void hide();
    void show();

    void commitWidget();
    void decomissionWidget();
    bool widgetIsInOperation();

    void teardown();
signals:
    void done();
};

class SlidingAnimator::ResizeAwareWidget : public QWidget {
    Q_OBJECT
public:
    explicit ResizeAwareWidget(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
    void resized();

private:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        emit resized();
    }
};

#endif// SLIDINGANIMATOR_H
