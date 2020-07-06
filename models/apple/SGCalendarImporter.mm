//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import <TargetConditionals.h>

#import "SGCalendarImporter.h"
#import "SGCalendarManager.h"

#if !TARGET_OS_IOS

#import <AppKit/NSColor.h>

#endif

#include "strategy.h"

NSString *const SGCalendarImporterKeyOptions = @"importOptions";
NSString *const SGCalendarImporterKeyCalendarsIdentifiers = @"importCalendarsIdentifiers";

@implementation SGCalendarImporterSettings

@end

@interface SGCalendarImporter () {
    stg::strategy *strategy;
}

@property(nonatomic, strong) SGCalendarImporterSettings *settings;
@property(nonatomic, strong) SGCalendarManager *calendarManager;

@end

@implementation SGCalendarImporter

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarImporterSettings *)settings {
    if (self = [super init]) {
        strategy = reinterpret_cast<stg::strategy *>(strategyPtr);
        self.settings = settings;
        self.calendarManager = [[SGCalendarManager alloc] initWithStore:store];
    }

    return self;
}

- (void)import {
    NSDate *date = self.settings.date != nil ? self.settings.date : [[NSDate alloc] init];
    NSArray<NSString *> *calendarsIdentifiers = self.settings.calendarsIdentifiers;

    if (calendarsIdentifiers != nil && calendarsIdentifiers.count == 0) {
        return;
    }

    NSArray<EKEvent *> *ekEvents = [self.calendarManager eventsWithDate:date
                                                   calendarsIdentifiers:calendarsIdentifiers];
    std::vector<stg::strategy::event> events;

    for (EKEvent *ekEvent in ekEvents) {
        NSDate *today = [NSCalendar.currentCalendar startOfDayForDate:date];

        auto beginTime = static_cast<stg::strategy::time_t>([ekEvent.startDate timeIntervalSinceDate:today] / 60);
        auto endTime = static_cast<stg::strategy::time_t>([ekEvent.endDate timeIntervalSinceDate:today] / 60);

#if !TARGET_OS_IOS
        CGColorRef color = ekEvent.calendar.color.CGColor;
#else
        CGColorRef color = ekEvent.calendar.CGColor;
#endif

        auto event = stg::strategy::event{stg::to_string(ekEvent.calendar.title),
                                          color,
                                          beginTime,
                                          endTime};

        events.push_back(event);
    }

    auto overwrite = (self.settings.optionsMask & SGCalendarImportOptionsOverwrite) == SGCalendarImportOptionsOverwrite;
    strategy->import_events(events, overwrite);
}

+ (void)importToStrategyPtr:(void *)strategyPtr
                       date:(NSDate *)date
          completionHandler:(void (^)(BOOL granted))completionHandler {
    [SGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
        if (!store) {
            if (completionHandler)
                completionHandler(false);

            return;
        }

        SGCalendarImporterSettings *settings = [SGCalendarImporter defaultSettings];
        settings.date = date;

        SGCalendarImporter *calendarImporter = [[SGCalendarImporter alloc] initWithStrategyPtr:strategyPtr
                                                                                    eventStore:store
                                                                                      settings:settings];

        [calendarImporter import];

        if (completionHandler)
            completionHandler(true);
    }];
}

+ (void)saveDefaultSettings:(SGCalendarImporterSettings *)settings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;

    [userDefaults setInteger:settings.optionsMask forKey:SGCalendarImporterKeyOptions];
    [userDefaults setObject:settings.calendarsIdentifiers forKey:SGCalendarImporterKeyCalendarsIdentifiers];
}

+ (SGCalendarImporterSettings *)defaultSettings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;

    SGCalendarImporterSettings *settings = [[SGCalendarImporterSettings alloc] init];
    settings.optionsMask = (SGCalendarImportOptions) [userDefaults integerForKey:SGCalendarImporterKeyOptions];
    settings.calendarsIdentifiers = (NSArray<NSString *> *) [userDefaults objectForKey:SGCalendarImporterKeyCalendarsIdentifiers];

    return settings;
}

@end
