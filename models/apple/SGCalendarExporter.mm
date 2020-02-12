//
// Created by Dmitry Khrykin on 2020-02-10.
//

#import "SGCalendarExporter.h"
#import "SGCalendarManager.h"
#import "NSColor+HexColor.h"

#include "strategy.h"

@implementation SGCalendarExporterSettings
@end

@interface SGCalendarExporter ()
@property(nonatomic) stg::strategy *strategy;
@property(nonatomic, strong) EKEventStore *eventStore;
@property(nonatomic, strong) SGCalendarManager *calendarManager;
@end

@implementation SGCalendarExporter
+ (void)requestExport:(void (^)(EKEventStore *store))completionHandler {
    EKAuthorizationStatus authorizationStatus = [EKEventStore authorizationStatusForEntityType:EKEntityTypeEvent];
    EKEventStore *store = [[EKEventStore alloc] init];

    if (authorizationStatus != EKAuthorizationStatusAuthorized) {
        [store requestAccessToEntityType:EKEntityTypeEvent completion:^(BOOL granted, NSError *error) {
            if (!granted) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    completionHandler(nil);
                });

                return;
            }

            dispatch_async(dispatch_get_main_queue(), ^{
                completionHandler(store);
            });
        }];
    } else {
        completionHandler(store);
    }
}

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarExporterSettings *)settings {
    if (self = [super init]) {
        self.strategy = reinterpret_cast<stg::strategy *>(strategyPtr);
        self.eventStore = store;
        self.settings = settings;

        self.calendarManager = [[SGCalendarManager alloc] initWithStore:self.eventStore];
    }

    return self;
}

- (void)export:(void (^)())completionHandler; {
    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    dispatch_async(queue, ^{
        NSDate *date = self.settings.date;
        SGCalendarExportOptions optionsMask = self.settings.optionsMask;

        auto &strategy = *self.strategy;
        auto nonEmptySessions = strategy.sessions().get_non_empty();
        auto numberOfEvents = static_cast<unsigned int>(nonEmptySessions.size());

        if ((optionsMask & SGCalendarExportOptionsOverwriteIsOn) == SGCalendarExportOptionsOverwriteIsOn) {
            NSString *calendarName = self.settings.calendarName;

            [self.calendarManager removeAllEventsForDate:date andCalendarName:calendarName];
        }

        if ((optionsMask & SGCalendarExportOptionsSpecificCalendarIsOn) ==
            SGCalendarExportOptionsSpecificCalendarIsOn) {
            self.calendarManager.calendarName = self.settings.calendarName;
        }

        if (nonEmptySessions.empty()) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                [self.delegate calendarManagerProgressChanged:1.0];
            });
        } else {
            for (auto &session : nonEmptySessions) {
                if (!session.activity)
                    continue;

                [self exportSession:&session];

                auto currentEventIndex = static_cast<unsigned >(&session - &nonEmptySessions[0]);
                auto progress = static_cast<float>(currentEventIndex + 1) / numberOfEvents;

                dispatch_sync(dispatch_get_main_queue(), ^{
                    [self.delegate calendarManagerProgressChanged:progress];
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
    BOOL includeNotifications = (self.settings.optionsMask & SGCalendarExportOptionsNotificationsIsOn) ==
                                SGCalendarExportOptionsNotificationsIsOn;

    NSColor *color = nil;
    NSString *calendarTitle = self.calendarManager.calendarName;
    if (!calendarTitle) {
        calendarTitle = [NSString stringWithUTF8String:session.activity->name().c_str()];
        color = [NSColor colorWithHexColorString:
                [NSString stringWithUTF8String:session.activity->color().c_str()]];
    }

    EKCalendar *calendar = [self.calendarManager findOrCreateCalendarWithTitle:calendarTitle
                                                                      andColor:color];

    NSString *eventTitle = self.calendarManager.calendarName != nil
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