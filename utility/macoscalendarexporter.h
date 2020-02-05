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
    using OptionsWindowResult = std::tuple<Response, Options, time_t, std::string>;

    static Option OverridePreviousEvents = 1u << 0u;
    static Option IncludeNotifications = 1u << 1u;
    static Option ExportToSpecificCalendar = 1u << 2u;

    static const Options defaultOptions = OverridePreviousEvents |
                                          IncludeNotifications;

    static MacOSCalendarExporter::OptionsWindowResult showOptionsAlert(Options initialOptions = defaultOptions,
                                                                       const std::string &initialCalendarName = "");

    static void
    exportStrategy(const stg::strategy &strategy,
                   Options options,
                   time_t dateSecsFromEpoch,
                   const std::string &calendarTitle = nullptr);
private:
    static bool optionEnabled(Options optionsMask,
                              Options setting);

    static void exportSession(const stg::session &strategy,
                              SGCalendarManager *calendarManager,
                              Options options,
                              time_t dateSecsFromEpoch);

    static void exportStrategyUnauthorized(EKEventStore *store, const stg::strategy &strategy, Options options,
                                           time_t dateSecsFromEpoch, const std::string &calendarTitle);
    static void showAccessDeniedAlert();
};


#endif //STRATEGR_MACOSCALENDAREXPORTER_H
