//
// Created by Dmitry Khrykin on 2019-12-25.
//

#ifndef STRATEGR_SEARCHBOXWIDGET_H
#define STRATEGR_SEARCHBOXWIDGET_H

#include <QWidget>

#include "colorprovider.h"
#include "cursorprovider.h"

class QLineEdit;

class SearchBoxWidget : public QWidget,
                        private ColorProvider,
                        private CursorProvider {
    Q_OBJECT
public:
    explicit SearchBoxWidget(const QString &placeholder, QWidget *parent = nullptr);

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

    QRect iconRect() const;

    void reloadPalette() const;
};


#endif//STRATEGR_SEARCHBOXWIDGET_H
