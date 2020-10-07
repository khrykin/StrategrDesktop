#ifndef ABSTRACTSPINBOXDECORATOR_H
#define ABSTRACTSPINBOXDECORATOR_H

#include <QLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QTimeEdit>
#include <QWidget>

class AbstractSpinBoxDecorator : public QWidget {
    Q_OBJECT
public:
    explicit AbstractSpinBoxDecorator(QAbstractSpinBox *_abstractSpinBox,
                                      QWidget *parent = nullptr);

    ~AbstractSpinBoxDecorator() override = default;

    int getFontPixelSize() const;
    void setFontPixelSize(int value);

public slots:
    void stepUp();
    void stepDown();

private:
    static const auto buttonsWidth = 19;
    static const auto buttonsHeight = buttonsWidth / 2;
    static const auto defaultFontPixelSize = 14;

    QVBoxLayout *buttonsLayout;
    QWidget *buttonsWidget;
    QAbstractSpinBox *_abstractSpinBox;
    QPushButton *upButton;
    QPushButton *downButton;
    int fontPixelSize = defaultFontPixelSize;

    void setup();
    void applyStyleSheet();

    void createLayout();
    void createButtonsLayout();
    void createButtonsWidget();
    void createButtons();

    void addSpinboxToLayout();

    QPushButton *makeButton(QString text = "");

protected:
    virtual void connectImplementation() = 0;
    QAbstractSpinBox *abstractSpinBox() const;
};

class SpinBoxDecorator : public AbstractSpinBoxDecorator {
    Q_OBJECT

    void connectImplementation() override {
        connect(spinBox(), QOverload<int>::of(&QSpinBox::valueChanged), this,
                &SpinBoxDecorator::valueChanged);
    }

public:
    using AbstractSpinBoxDecorator::AbstractSpinBoxDecorator;

    QSpinBox *spinBox() {
        return qobject_cast<QSpinBox *>(abstractSpinBox());
    }

public slots:

    void valueChanged(int value) { spinBox()->setValue(value); }
};

class TimeEditDecorator : public AbstractSpinBoxDecorator {
    Q_OBJECT

    void connectImplementation() override {
        connect(timeEdit(), &QTimeEdit::timeChanged, this,
                &TimeEditDecorator::timeChanged);
    }

public:
    using AbstractSpinBoxDecorator::AbstractSpinBoxDecorator;

    QTimeEdit *timeEdit() {
        return qobject_cast<QTimeEdit *>(abstractSpinBox());
    }

public slots:

    void timeChanged(QTime time) {
        timeEdit()->setTime(time);
    }
};

#endif// ABSTRACTSPINBOXDECORATOR_H
