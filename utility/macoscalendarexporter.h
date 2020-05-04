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
    enum class Response {
        Cancel = 0,
        Perform = 1
    };

    using Options = unsigned;
    using Option = const unsigned;

    struct ExportOptionsWindowResult {
        Response response;
        Options options = 0;
        time_t dateTimestamp = 0;
        std::string calendarName;
    };

    struct ImportOptionsWindowResult {
        Response response;
        Options options = 0;
        time_t dateTimestamp = 0;
        std::unique_ptr<std::vector<std::string>> calendarsIdentifiers;
    };

    static Option OverridePreviousEvents = 1u << 0u;
    static Option IncludeNotifications = 1u << 1u;
    static Option ExportToSpecificCalendar = 1u << 2u;

    static const Options defaultOptions = OverridePreviousEvents |
                                          IncludeNotifications;

    static ExportOptionsWindowResult showExportOptionsWindow(Options initialOptions = defaultOptions,
                                                             const std::string &initialCalendarName = "");


    static ImportOptionsWindowResult showImportOptionsWindow(Options initialOptions = defaultOptions,
                                                             std::unique_ptr<std::vector<std::string>> initialCalendarsIdentifiers = nullptr);

    static void exportStrategy(const stg::strategy &strategy,
                               Options options,
                               time_t dateSecsFromEpoch,
                               const std::string &calendarTitle = nullptr);
private:
    static void showAccessDeniedAlert();
};


#endif //STRATEGR_MACOSCALENDAREXPORTER_H
