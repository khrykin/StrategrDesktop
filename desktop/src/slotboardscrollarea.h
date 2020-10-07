//
// Created by Dmitry Khrykin on 25.05.2020.
//

#ifndef STRATEGR_SLOTBOARDSCROLLAREA_H
#define STRATEGR_SLOTBOARDSCROLLAREA_H

#include <QScrollArea>

class SlotboardScrollArea : public QScrollArea {
public:
    using QScrollArea::QScrollArea;

    void setViewportTopOffset(int top);
    void setScrollOffset(int top);

    QRect viewportRectRelativeToContent();
    QRect viewportRect();

private:
    int viewportTopOffset = 0;

    void fixScrollBar();

    void resizeEvent(QResizeEvent *) override;
    void wheelEvent(QWheelEvent *) override;
};


#endif//STRATEGR_SLOTBOARDSCROLLAREA_H
