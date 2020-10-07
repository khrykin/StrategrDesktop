//
// Created by Dmitry Khrykin on 04.10.2020.
//

#ifndef STRATEGR_DRAWINGUTILS_H
#define STRATEGR_DRAWINGUTILS_H

#include <QPainter>

namespace DrawingUtils {

    inline constexpr double squircleDefaultSquareness = 0.1;

    inline QPainterPath squirclePath(const QRect &rect, int radius, double squareness = squircleDefaultSquareness) {
        const auto yRadiusPoint = 2 * QPoint(0, radius);
        const auto xRadiusPoint = 2 * QPoint(radius, 0);

        using CornerConfig = struct {
            QPoint p1;
            QPoint c1;
            QPoint c2;
            QPoint p2;
        };

        struct {
            CornerConfig topLeft;
            CornerConfig topRight;
            CornerConfig bottomRight;
            CornerConfig bottomLeft;
        } controlPoints;

        const auto bottomFixY = QPoint(0, 1);
        const auto rightFixX = QPoint(1, 0);

        controlPoints.topLeft.p1 = rect.topLeft() + yRadiusPoint;
        controlPoints.topLeft.c1 = rect.topLeft() + squareness * yRadiusPoint;
        controlPoints.topLeft.c2 = rect.topLeft() + squareness * xRadiusPoint;
        controlPoints.topLeft.p2 = rect.topLeft() + xRadiusPoint;

        controlPoints.topRight.p1 = rightFixX + rect.topRight() - xRadiusPoint;
        controlPoints.topRight.c1 = rightFixX + rect.topRight() - squareness * xRadiusPoint;
        controlPoints.topRight.c2 = rightFixX + rect.topRight() + squareness * yRadiusPoint;
        controlPoints.topRight.p2 = rightFixX + rect.topRight() + yRadiusPoint;

        controlPoints.bottomRight.p1 = rightFixX + bottomFixY + rect.bottomRight() - yRadiusPoint;
        controlPoints.bottomRight.c1 = rightFixX + bottomFixY + rect.bottomRight() - squareness * yRadiusPoint;
        controlPoints.bottomRight.c2 = rightFixX + bottomFixY + rect.bottomRight() - squareness * xRadiusPoint;
        controlPoints.bottomRight.p2 = rightFixX + bottomFixY + rect.bottomRight() - xRadiusPoint;

        controlPoints.bottomLeft.p1 = bottomFixY + rect.bottomLeft() + xRadiusPoint;
        controlPoints.bottomLeft.c1 = bottomFixY + rect.bottomLeft() + squareness * xRadiusPoint;
        controlPoints.bottomLeft.c2 = bottomFixY + rect.bottomLeft() - squareness * yRadiusPoint;
        controlPoints.bottomLeft.p2 = bottomFixY + rect.bottomLeft() - yRadiusPoint;

        if (rect.height() < 2 * yRadiusPoint.y()) {
            const auto leftMidY = rect.topLeft().y() + rect.height() / 2;

            controlPoints.topLeft.p1.setY(leftMidY);
            controlPoints.bottomLeft.p2.setY(leftMidY);

            const auto rightMidY = rect.topRight().y() + rect.height() / 2;

            controlPoints.topRight.p2.setY(rightMidY);
            controlPoints.bottomRight.p1.setY(rightMidY);
        }

        if (rect.width() < 2 * xRadiusPoint.x()) {
            const auto topMidX = rect.topLeft().x() + rect.width() / 2;

            controlPoints.topLeft.p2.setX(topMidX);
            controlPoints.bottomLeft.p1.setX(topMidX);

            const auto bottomMidX = rect.topRight().x() - rect.width() / 2;

            controlPoints.topRight.p1.setX(bottomMidX);
            controlPoints.bottomRight.p2.setX(bottomMidX);
        }

        auto path = QPainterPath();
        path.moveTo(controlPoints.topLeft.p1);
        path.cubicTo(controlPoints.topLeft.c1,
                     controlPoints.topLeft.c2,
                     controlPoints.topLeft.p2);

        path.lineTo(controlPoints.topRight.p1);
        path.cubicTo(controlPoints.topRight.c1,
                     controlPoints.topRight.c2,
                     controlPoints.topRight.p2);

        path.lineTo(controlPoints.bottomRight.p1);
        path.cubicTo(controlPoints.bottomRight.c1,
                     controlPoints.bottomRight.c2,
                     controlPoints.bottomRight.p2);

        path.lineTo(controlPoints.bottomLeft.p1);
        path.cubicTo(controlPoints.bottomLeft.c1,
                     controlPoints.bottomLeft.c2,
                     controlPoints.bottomLeft.p2);

        path.lineTo(controlPoints.topLeft.p1);

        return path;
    }
}

#endif//STRATEGR_DRAWINGUTILS_H
