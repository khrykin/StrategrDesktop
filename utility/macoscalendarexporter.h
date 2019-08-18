//
// Created by Dmitry Khrykin on 2019-08-15.
//

#ifndef STRATEGR_MACOSCALENDAREXPORTER_H
#define STRATEGR_MACOSCALENDAREXPORTER_H

#include <functional>
#include <optional>
#include <tuple>
#include <ctime>

#include "strategy.h"

#ifdef __OBJC__
#define OBJC_CLASS(name) @class name
#else
#define OBJC_CLASS(name) typedef struct objc_object name
#endif

OBJC_CLASS(SGCalendarManager);
OBJC_CLASS(EKEventStore);

class MacOSCalendarExporter {
public:
    enum class Response {
        Cancel = 0,
        Export = 1
    };

    using Options = unsigned;
    using Option = const unsigned;
    using OptionsWindowResult = std::tuple<Response, Options, time_t>;

    static Option OverridePreviousEvents = 0x1;
    static Option IncludeNotifications = 0x2;

    static const Options defaultOptions = OverridePreviousEvents |
                                          IncludeNotifications;

    static OptionsWindowResult
    showOptionsAlert(Options initialOptions = defaultOptions);

    static void exportStrategy(const Strategy &strategy,
                               Options options,
                               time_t dateSecsFromEpoch);
private:
    static bool optionEnabled(Options optionsMask,
                              Options setting);

    static void exportSession(const ActivitySession &strategy,
                              SGCalendarManager *calendarManager,
                              Options options,
                              time_t dateSecsFromEpoch);

    static void exportStrategyUnauthorized(EKEventStore *store,
                                           const Strategy &strategy, Options options,
                                           time_t dateSecsFromEpoch);
    static void showAccessDeniedAlert();
};


#endif //STRATEGR_MACOSCALENDAREXPORTER_H
