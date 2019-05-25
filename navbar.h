#ifndef NAVSCREEN_H
#define NAVSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QPushButton>

class Navbar : public QWidget
{
    Q_OBJECT

signals:

public slots:

private:
    QString _title;

    QPushButton *_leftButton;
    QPushButton *_rightButton;
    QLabel *_titleLabel;

    QWidget *navbar;
    QWidget *contentWidget;

    void paintEvent(QPaintEvent *) override;

public:
    explicit Navbar(QWidget *parent = nullptr);

    QString title() const;
    void setTitle(const QString &title);

    template <typename Method>
    void setLeftButton(const QString &text,
                       const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
                       Method slot)
    {
        _leftButton->setText(text);
        QObject::connect(_leftButton, &QPushButton::clicked, receiver, slot);
    }

    template <typename Method>
    void setRightButton(const QString &text,
                        const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
                        Method slot)
    {
        _rightButton->setText(text);
        QObject::connect(_rightButton, &QPushButton::clicked, receiver, slot);
    }
};

#endif // NAVSCREEN_H
