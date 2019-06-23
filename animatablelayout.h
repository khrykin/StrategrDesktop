#include <QLayout>

QT_FORWARD_DECLARE_CLASS(QPropertyAnimation)

class AnimatableLayout : public QLayout {
  Q_OBJECT

  Q_PROPERTY(QPoint delta READ delta WRITE setDelta NOTIFY deltaChanged)

  Q_PROPERTY(QRect widgetRect READ widgetRect WRITE setWidgetRect NOTIFY
                 widgetRectChanged)

  Q_PROPERTY(bool active READ isDeltaActive WRITE setDeltaActive NOTIFY
                 deltaActiveChanged)

public:
  explicit AnimatableLayout(QWidget *parent = nullptr);
  ~AnimatableLayout();

  QPoint delta() const;
  void setDelta(const QPoint &value);

  QSize sizeHint() const;
  void setGeometry(const QRect &);
  QSize minimumSize() const;
  int count() const;
  QSize deltaSize() const;

  QRect widgetRect() const;
  void setWidgetRect(const QRect &value);

  bool isDeltaActive() const;
  void setDeltaActive(bool active = true);

  void updateItemPosition();

private:
  void addItem(QLayoutItem *item);
  QLayoutItem *itemAt(int index) const;
  QLayoutItem *takeAt(int index);

signals:
  void deltaChanged(const QPoint &value);
  void widgetRectChanged(const QRect &value);
  void deltaActiveChanged(bool active);

public slots:
  void testIt();

private:
  QLayoutItem *item;
  QPropertyAnimation *animation;
  QPoint mDelta;
  bool mDeltaActive;
};
