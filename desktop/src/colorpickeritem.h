#ifndef COLORPICKERITEM_H
#define COLORPICKERITEM_H

#include <QColor>
#include <QWidget>

class ColorPickerItem : public QWidget {
    Q_OBJECT
public:
    explicit ColorPickerItem(QColor color,
                             QWidget *parent = nullptr);

    QColor color() const;
    void setColor(const QColor &color);

    bool isSelected() const;
    void setIsSelected(bool isSelected);
    void toggleIsSelected();

signals:
    void clicked(QColor color);

public slots:

private:
    bool isClicked = false;
    bool _isSelected = false;
    bool isHovered = false;
    QColor _color;

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif// COLORPICKERITEM_H
