//
// Created by Dmitry Khrykin on 2019-07-26.
//

#ifndef STRATEGR_ROWWIDGET_H
#define STRATEGR_ROWWIDGET_H

#include <QWidget>
#include "colorprovider.h"

class RowWidget : public QWidget, public ColorProvider {
public:
    using QWidget::QWidget;
    bool isHardBorder() const;
    void setIsHardBorder(bool isHardBorder);
private:
    void paintEvent(QPaintEvent *event) override;
    bool _isHardBorder = false;
};


#endif //STRATEGR_ROWWIDGET_H
