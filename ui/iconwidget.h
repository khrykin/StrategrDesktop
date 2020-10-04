//
// Created by Dmitry Khrykin on 2020-02-08.
//

#ifndef STRATEGR_ICONWIDGET_H
#define STRATEGR_ICONWIDGET_H

#include <QWidget>

class IconWidget : public QWidget {
    Q_OBJECT
public:
    static QString backCode();
    static QString addCode();

    explicit IconWidget(const QString &code = "", QWidget *parent = nullptr);
    const QString &code() const;
    void setCode(const QString &code);

signals:
    void clicked();

private:
    QString _code = "";

public:
    void setColor(const QColor &color);

private:
    bool isPressed = false;
    QColor _color;

    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

    void paintEvent(QPaintEvent *) override;
};


#endif//STRATEGR_ICONWIDGET_H
