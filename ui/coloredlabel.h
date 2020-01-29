//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_COLOREDLABEL_H
#define STRATEGR_COLOREDLABEL_H

#include <functional>
#include <QLabel>

class ColoredLabel : public QWidget {
Q_OBJECT
public:
    std::function<void(QPainter *, const QString &)> customRenderer = nullptr;

    explicit ColoredLabel(QString text = "", QWidget *parent = nullptr);

    const QColor &color() const;
    void setColor(const QColor &color);

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
    std::function<QColor()> colorGetter = nullptr;
    Qt::TextElideMode _elideMode = Qt::ElideNone;

    QColor _color;
    QString _text;
    Qt::Alignment _alignment = Qt::AlignLeft | Qt::AlignVCenter;

    void paintEvent(QPaintEvent *event) override;
};


#endif //STRATEGR_COLOREDLABEL_H
