//
// Created by Dmitry Khrykin on 2019-12-25.
//

#ifndef STRATEGR_SEARCHBOX_H
#define STRATEGR_SEARCHBOX_H

#include <QWidget>

#include "colorprovider.h"

class QLineEdit;

class SearchBox : public QWidget, public ColorProvider {
Q_OBJECT
public:
    explicit SearchBox(const QString &placeholder,
                       QWidget *parent = nullptr);

    QString text();
    void setText(const QString &string);

    void focus();
    void removeFocus();

signals:
    void textEdited(const QString &);
    void gotFocus();
    void lostFocus();
protected:
    void paintEvent(QPaintEvent *) override;
    bool eventFilter(QObject *object, QEvent *event) override;
private:
    QLineEdit *lineEdit;

    QAction *cancelAction = nullptr;
    QAction *selectUpAction = nullptr;
    QAction *selectDownAction = nullptr;

    static constexpr auto iconText = "\uf002";
    QFont iconFont() const;
    QRect iconRect() const;

    void reloadPalette() const;
};


#endif //STRATEGR_SEARCHBOX_H
