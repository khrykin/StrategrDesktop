//
// Created by Dmitry Khrykin on 2020-02-10.
//

#import "SGCalendarExporter.h"
#import "SGCalendarManager.h"

#include "strategy.h"

NSString *const SGCalendarExporterKeyOptions = @"exportOptions";
NSString *const SGCalendarExporterKeyCalendarName = @"exportCalendarName";

@implementation SGCalendarExporterSettings
@end

@interface SGCalendarExporter () {
    stg::strategy *strategy;
}

@property(nonatomic, strong) SGCalendarExporterSettings *settings;
@property(nonatomic, strong) SGCalendarManager *calendarManager;
@property(nonatomic, readonly) BOOL usingSpecificCalendar;

@end

@implementation SGCalendarExporter

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarExporterSettings *)settings {
    if (self = [super init]) {
        strategy = reinterpret_cast<stg::strategy *>(strategyPtr);
        self.settings = settings;
        self.calendarManager = [[SGCalendarManager alloc] initWithStore:store];
    }

    return self;
}

- (BOOL)usingSpecificCalendar {
    return (self.settings.optionsMask & SGCalendarExportOptionsUseSpecificCalendar) ==
           SGCalendarExportOptionsUseSpecificCalendar;
}

- (NSDate *)date {
    if (!self.settings.date) {
        return [NSDate date];
    }

    return self.settings.date;
}

- (void)export:(void (^)())completionHandler; {
    dispatch_queue_t queue = dispatch_get_global_queue(0, 0);
    dispatch_async(queue, ^{
        SGCalendarExportOptions optionsMask = self.settings.optionsMask;

        auto nonEmptySessions = self->strategy->sessions().get_non_empty();
        auto numberOfEvents = static_cast<unsigned int>(nonEmptySessions.size());

        NSString *calendarName = self.usingSpecificCalendar ? self.settings.calendarName : nil;

        if ((optionsMask & SGCalendarExportOptionsOverwrite) == SGCalendarExportOptionsOverwrite) {
            [self.calendarManager removeAllEventsForDate:self.date andCalendarName:calendarName];
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

    NSString *calendarName = self.usingSpecificCalendar ? self.settings.calendarName : nil;

    if (!calendarName) {
        calendarName = stg::to_nsstring(session.activity->name());
        color = session.activity->color();
    }

    EKCalendar *calendar = [self.calendarManager findOrCreateCalendarWithTitle:calendarName
                                                                      andColor:color];

    [self.calendarManager createEventForCalendar:calendar
                                            date:date
                                           title:stg::to_nsstring(session.activity->name())
                                    beginMinutes:session.begin_time()
                                 durationMinutes:session.duration()
                            includeNotifications:includeNotifications];
}

+ (void)exportFromStrategyPtr:(void *)strategyPtr
                         date:(NSDate *_Nullable)date
                     delegate:(nullable id <SGCalendarExporterDelegate>)delegate
            completionHandler:(nullable void (^)(BOOL exported))completionHandler {
    [SGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store) {
            if (completionHandler)
                completionHandler(false);

            return;
        }

        SGCalendarExporterSettings *settings = [SGCalendarExporter defaultSettings];
        settings.date = date;

        SGCalendarExporter *exporter = [[SGCalendarExporter alloc] initWithStrategyPtr:strategyPtr
                                                                            eventStore:store
                                                                              settings:settings];

        exporter.delegate = delegate;

        [exporter export:^{
            [SGCalendarManager launchCalendarAppWithDate:exporter.date];

            if (completionHandler)
                completionHandler(true);
        }];
    }];
}

+ (void)saveDefaultSettings:(SGCalendarExporterSettings *)settings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;
    [userDefaults setInteger:settings.optionsMask forKey:SGCalendarExporterKeyOptions];

    if (settings.calendarName)
        [userDefaults setObject:settings.calendarName forKey:SGCalendarExporterKeyCalendarName];
}

+ (SGCalendarExporterSettings *)defaultSettings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;
    SGCalendarExporterSettings *settings = [[SGCalendarExporterSettings alloc] init];
    settings.optionsMask = (SGCalendarExportOptions) [userDefaults integerForKey:SGCalendarExporterKeyOptions];
    settings.calendarName = (NSString *) [userDefaults objectForKey:SGCalendarExporterKeyCalendarName];
    settings.date = [NSDate date];

    return settings;
}

@end
