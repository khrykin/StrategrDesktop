#ifndef NAVSCREEN_H
#define NAVSCREEN_H

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "colorprovider.h"
#include "iconwidget.h"

#define DEBUG_NAVBAR false

#if defined(Q_OS_MAC) && !DEBUG_NAVBAR
#define HIDE_NAVBAR
#endif

class Navbar : public QWidget, public ColorProvider {
    Q_OBJECT
public:
    explicit Navbar(QWidget *parent = nullptr);

    void setTitle(const QString &title);

    QWidget *leftButton() const;
    QWidget *rightButton() const;
    QLabel *titleLabel() const;

#ifdef HIDE_NAVBAR

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
        _leftButton->setCode(text);
        QObject::connect(_leftButton, &IconWidget::clicked, receiver, slot);
    }

    template<typename Method>
    void setRightButton(
        const QString &text,
        const typename QtPrivate::FunctionPointer<Method>::Object *receiver,
        Method slot) {
        _rightButton->setCode(text);
        QObject::connect(_rightButton, &IconWidget::clicked, receiver, slot);
    }

private:
    QString _title;

    IconWidget *_leftButton = nullptr;
    IconWidget *_rightButton = nullptr;
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

#endif// NAVSCREEN_H
