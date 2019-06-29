#ifndef SLIDINGANIMATOR_H
#define SLIDINGANIMATOR_H

#include <QLayout>
#include <QTimeLine>
#include <QWidget>

class SlidingAnimator : public QObject {
  Q_OBJECT

  class ResizeAwareWidget;
  enum class Orientation { Horizontal, Vertical };
  enum class Operation { Show, Hide };

  using WidgetSizeSetterPointer = void (QWidget::*)(int);
  template <class T> using SizeGetterPointer = int (T::*)() const;

  static const auto duration = 150;
  static const auto updateInterval = 20;

  static QVector<QWidget *> widgetsInOperation;

  QWidget *_widget;
  QBoxLayout *_widgetParentLayout;
  QTimeLine *timeLine;

  Orientation _orientation = Orientation::Vertical;
  Operation operation;

  int initialWidgetMinimumSize;
  int initialWidgetMaximumSize;

  int initialWidgetSize;
  int indexInParentLayout;

  WidgetSizeSetterPointer fixedSizeSetter();
  WidgetSizeSetterPointer maximumSizeSetter();
  WidgetSizeSetterPointer minimumSizeSetter();

  template <class T> SizeGetterPointer<T> sizeGetter() {
    if (orientation() == Orientation::Vertical) {
      return &T::height;
    } else {
      return &T::width;
    }
  }

  QPoint makePointBySustraction(QPoint point, int translation);
  QSize applyInitialSizeToRect(QRect rect, int initialSize);

  void setWidgetMaximumSize();
  void resetWidgetMaximumSize();

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

  SlidingAnimator(QWidget *widget = nullptr);
  ~SlidingAnimator();

  void hide();
  void show();

  void commitWidget();
  void decomissionWidget();
  bool widgetIsInOperation();

  void teardown();

signals:
  void done();

public:
  static void hideWidget(QWidget *widget);
  static void showWidget(QWidget *widget);

  QWidget *widget() const;

  Orientation orientation() const;
  void setOrientation(const Orientation &orientation);
signals:

public slots:
};

class SlidingAnimator::ResizeAwareWidget : public QWidget {
  Q_OBJECT

  void resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    emit resized();
  }

public:
  explicit ResizeAwareWidget(QWidget *parent = nullptr) : QWidget(parent) {}
signals:
  void resized();
};

#endif // SLIDINGANIMATOR_H
