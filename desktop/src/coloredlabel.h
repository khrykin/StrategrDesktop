//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_COLOREDLABEL_H
#define STRATEGR_COLOREDLABEL_H

#include <QLabel>
#include <functional>

class ColoredLabel : public QWidget {
public:
    std::function<void(QPainter *, const QString &)> customRenderer = nullptr;

    explicit ColoredLabel(QString text = "", QWidget *parent = nullptr);

    const QColor &color() const;

    QColor dynamicColor();
    void setDynamicColor(const std::function<QColor()> &newColorGetter);

    const QString &text() const;
    void setText(const QString &text);

    const Qt::Alignment &alignment() const;
    void setAlignment(const Qt::Alignment &alignment);

    void setBold(bool isBold);
    void setFontHeight(int fontHeight);

    Qt::TextElideMode elideMode() const;
    void setElideMode(Qt::TextElideMode elideMode);

    QSize sizeHint() const override;

private:
    Qt::TextElideMode _elideMode = Qt::ElideNone;
    std::function<QColor()> colorGetter = [] { return Qt::black; };

    QColor _color;
    QString _text;
    Qt::Alignment _alignment = Qt::AlignLeft | Qt::AlignVCenter;

    void paintEvent(QPaintEvent *event) override;
};


#endif//STRATEGR_COLOREDLABEL_H
