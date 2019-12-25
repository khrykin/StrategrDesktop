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

signals:
    void textEdited(const QString &);
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QLineEdit *lineEdit;

    static constexpr auto iconText = "\uf002";
    QFont iconFont() const;
    QRect iconRect() const;
};


#endif //STRATEGR_SEARCHBOX_H
