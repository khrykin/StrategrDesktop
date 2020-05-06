//
// Created by Dmitry Khrykin on 2020-02-10.
//

#import "SGCalendarExporter.h"
#import "SGCalendarManager.h"

#include "strategy.h"

@implementation SGCalendarExporterSettings
@end

@interface SGCalendarExporter () {
    stg::strategy *strategy;
}

@property(nonatomic, strong) EKEventStore *eventStore;
@property(nonatomic, strong) SGCalendarManager *calendarManager;

@end

@implementation SGCalendarExporter

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarExporterSettings *)settings {
    if (self = [super init]) {
        strategy = reinterpret_cast<stg::strategy *>(strategyPtr);
        self.eventStore = store;
        self.settings = settings;

        self.calendarManager = [[SGCalendarManager alloc] initWithStore:store];
    }

    return self;
}

- (void)export:(void (^)())completionHandler; {
    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    dispatch_async(queue, ^{
        NSDate *date = self.settings.date;
        SGCalendarExportOptions optionsMask = self.settings.optionsMask;

        auto nonEmptySessions = strategy->sessions().get_non_empty();
        auto numberOfEvents = static_cast<unsigned int>(nonEmptySessions.size());

        if ((optionsMask & SGCalendarExportOptionsOverwrite) == SGCalendarExportOptionsOverwrite) {
            NSString *calendarName = self.settings.calendarName;

            [self.calendarManager removeAllEventsForDate:date andCalendarName:calendarName];
        }

        if (nonEmptySessions.empty()) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self.delegate calendarExporterProgressChanged:1.0];
            });
        } else {
            for (auto &session : nonEmptySessions) {
                if (!session.activity)
                    continue;

                [self exportSession:&session];

                auto currentEventIndex = static_cast<unsigned >(&session - &nonEmptySessions[0]);
                auto progress = static_cast<float>(currentEventIndex + 1) / numberOfEvents;

                dispatch_sync(dispatch_get_main_queue(), ^{
                    [self.delegate calendarExporterProgressChanged:progress];
                });
            }
        }

        dispatch_sync(dispatch_get_main_queue(), completionHandler);
    });
}

- (void)exportSession:(stg::session *)sessionPtr {
    auto &session = *sessionPtr;
    if (!session.activity)
        return;

    NSDate *date = self.settings.date;
    BOOL includeNotifications = (self.settings.optionsMask & SGCalendarExportOptionsIncludeNotifications) ==
                                SGCalendarExportOptionsIncludeNotifications;

    CGColorRef color = nil;

//        if ((optionsMask & SGCalendarExportOptionsUseSpecificCalendar) ==
//            SGCalendarExportOptionsUseSpecificCalendar) {
//            self.calendarManager.calendarName = self.settings.calendarName;
//        }

    NSString *calendarTitle = self.settings.calendarName;

    if (!calendarTitle) {
        calendarTitle = [NSString stringWithUTF8String:session.activity->name().c_str()];
        color = session.activity->color().to_cg_color();
    }

    EKCalendar *calendar = [self.calendarManager findOrCreateCalendarWithTitle:calendarTitle
                                                                      andColor:color];

    NSString *eventTitle = self.settings.calendarName != nil
                           ? [NSString stringWithUTF8String:session.activity->name().c_str()]
                           : calendarTitle;

    [self.calendarManager createEventForCalendar:calendar
                                            date:date
                                           title:eventTitle
                                    beginMinutes:session.begin_time()
                                 durationMinutes:session.duration()
                            includeNotifications:includeNotifications];
}

@end