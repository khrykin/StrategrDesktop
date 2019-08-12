//
// Created by Dmitry Khrykin on 2019-08-12.
//

#include "applicationicon.h"

#ifndef Q_OS_MAC
QPixmap ApplicationIcon::defaultIcon() {
    return QPixmap();
}
#endif
