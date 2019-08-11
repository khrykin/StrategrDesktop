//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_COLOREDLABEL_H
#define STRATEGR_COLOREDLABEL_H

#include <QLabel>
#include <functional>

class ColoredLabel : public QWidget {
public:

    explicit ColoredLabel(QString text = "", QWidget *parent = nullptr);

    const QColor &color() const;
    void setColor(const QColor &color);

    QColor dynamicColor();

    const QString &text() const;
    void setText(const QString &text);

    const Qt::Alignment &alignment() const;
    void setAlignment(const Qt::Alignment &alignment);

    void setBold(bool isBold);
    void setFontHeight(double fontHeight);

    void setDynamicColor(const std::function<QColor()> &newColorGetter);

    QSize sizeHint() const override;
private:
    std::function<QColor()> colorGetter = nullptr;
    QColor _color;
    QString _text;
    Qt::Alignment _alignment = Qt::AlignLeft | Qt::AlignVCenter;

    void paintEvent(QPaintEvent *event) override;
};


#endif //STRATEGR_COLOREDLABEL_H
