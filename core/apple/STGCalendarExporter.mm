//
// Created by Dmitry Khrykin on 2020-02-10.
//

#import "STGCalendarExporter.h"
#import "STGCalendarManager.h"

#include "../strategy.h"

NSString *const STGCalendarExporterKeyOptions = @"exportOptions";
NSString *const STGCalendarExporterKeyCalendarName = @"exportCalendarName";

@implementation STGCalendarExporterSettings
@end

@interface STGCalendarExporter () {
    const stg::strategy *strategy;
}

@property(nonatomic, strong) STGCalendarExporterSettings *settings;
@property(nonatomic, strong) STGCalendarManager *calendarManager;
@property(nonatomic, readonly) BOOL usingSpecificCalendar;

@end

@implementation STGCalendarExporter

- (instancetype)initWithStrategyPtr:(const stg::strategy *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(STGCalendarExporterSettings *)settings {
    if (self = [super init]) {
        self->strategy = strategyPtr;
        self.settings = settings;
        self.calendarManager = [[STGCalendarManager alloc] initWithStore:store];
    }

    return self;
}

- (BOOL)usingSpecificCalendar {
    return (self.settings.optionsMask & STGCalendarExportOptionsUseSpecificCalendar) ==
           STGCalendarExportOptionsUseSpecificCalendar;
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
        STGCalendarExportOptions optionsMask = self.settings.optionsMask;

        auto nonEmptySessions = self->strategy->sessions().get_non_empty();
        auto numberOfEvents = static_cast<unsigned int>(nonEmptySessions.size());

        NSString *calendarName = self.usingSpecificCalendar ? self.settings.calendarName : nil;

        if ((optionsMask & STGCalendarExportOptionsOverwrite) == STGCalendarExportOptionsOverwrite) {
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
    BOOL includeNotifications = (self.settings.optionsMask & STGCalendarExportOptionsIncludeNotifications) ==
                                STGCalendarExportOptionsIncludeNotifications;

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

+ (void)exportFromStrategyPtr:(const stg::strategy *)strategy
                         date:(NSDate *_Nullable)date
                     delegate:(nullable id <STGCalendarExporterDelegate>)delegate
            completionHandler:(nullable void (^)(BOOL exported))completionHandler {
    [STGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store) {
            if (completionHandler)
                completionHandler(false);

            return;
        }

        STGCalendarExporterSettings *settings = [STGCalendarExporter defaultSettings];
        settings.date = date;

        STGCalendarExporter *exporter = [[STGCalendarExporter alloc] initWithStrategyPtr:strategy
                                                                              eventStore:store
                                                                                settings:settings];

        exporter.delegate = delegate;

        [exporter export:^{
            [STGCalendarManager launchCalendarAppWithDate:exporter.date];

            if (completionHandler)
                completionHandler(true);
        }];
    }];
}

+ (void)saveDefaultSettings:(STGCalendarExporterSettings *)settings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;
    [userDefaults setInteger:settings.optionsMask forKey:STGCalendarExporterKeyOptions];

    if (settings.calendarName)
        [userDefaults setObject:settings.calendarName forKey:STGCalendarExporterKeyCalendarName];
}

+ (STGCalendarExporterSettings *)defaultSettings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;
    STGCalendarExporterSettings *settings = [[STGCalendarExporterSettings alloc] init];
    settings.optionsMask = (STGCalendarExportOptions) [userDefaults integerForKey:STGCalendarExporterKeyOptions];
    settings.calendarName = (NSString *) [userDefaults objectForKey:STGCalendarExporterKeyCalendarName];
    settings.date = [NSDate date];

    return settings;
}

@end
