#ifndef NAVSCREEN_H
#define NAVSCREEN_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "colorprovider.h"

#define DEBUG_NAVBAR false

#if !defined(Q_OS_MAC) || DEBUG_NAVBAR
#define SHOW_NAVBAR
#endif

class Navbar : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit Navbar(QWidget *parent = nullptr);

    void setTitle(const QString &title);

    QPushButton *leftButton() const;
    QPushButton *rightButton() const;
    QLabel *titleLabel() const;

#ifndef SHOW_NAVBAR

    template<typename Method>
    void setLeftButton(
            const QString &text,
            const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
            Method slot) {
    }

    template<typename Method>
    void setRightButton(
            const QString &text,
            const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
            Method slot) {

    }

#else
    QString title() const;

    template<typename Method>
    void setLeftButton(
            const QString &text,
            const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
            Method slot) {
        _leftButton->setText(text);
        QObject::connect(_leftButton, &QPushButton::clicked, receiver, slot);
    }

    template<typename Method>
    void setRightButton(
            const QString &text,
            const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
            Method slot) {
        _rightButton->setText(text);
        QObject::connect(_rightButton, &QPushButton::clicked, receiver, slot);
    }

private:
    QString _title;

    QPushButton *_leftButton = nullptr;
    QPushButton *_rightButton = nullptr;
    QLabel *_titleLabel = nullptr;

    QWidget *navbar = nullptr;
    QWidget *contentWidget = nullptr;

    void paintEvent(QPaintEvent *) override;

    void setupWidgets();
    void setupLayout();
    void setupTitleLabel();
    void setupLeftButton();
    void setupRightButton();

    void addWidgetsToLayout();
#endif
};

#endif // NAVSCREEN_H
