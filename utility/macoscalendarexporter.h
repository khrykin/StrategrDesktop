//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_MACOSCALENDAREXPORTER_H
#define STRATEGR_MACOSCALENDAREXPORTER_H

#include <functional>
#include <ctime>
#include <memory>

#include "strategy.h"

class MacOSCalendarExporter {
public:
    static void exportStrategy(const stg::strategy &strategy);
    static void importStrategy(stg::strategy &strategy);
private:
    static void showAccessDeniedAlert();
};


#endif //STRATEGR_MACOSCALENDAREXPORTER_H
