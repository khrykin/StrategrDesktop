//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import <TargetConditionals.h>

#import "STGCalendarImporter.h"
#import "STGCalendarManager.h"

#if !TARGET_OS_IOS

#import <AppKit/NSColor.h>

#endif

#include "../strategy.h"

NSString *const STGCalendarImporterKeyOptions = @"importOptions";
NSString *const STGCalendarImporterKeyCalendarsIdentifiers = @"importCalendarsIdentifiers";

@implementation STGCalendarImporterSettings

@end

@interface STGCalendarImporter () {
    stg::strategy *strategy;
}

@property(nonatomic, strong) STGCalendarImporterSettings *settings;
@property(nonatomic, strong) STGCalendarManager *calendarManager;

@end

@implementation STGCalendarImporter

- (instancetype)initWithStrategyPtr:(stg::strategy *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(STGCalendarImporterSettings *)settings {
    if (self = [super init]) {
        strategy = strategyPtr;

        self.settings = settings;
        self.calendarManager = [[STGCalendarManager alloc] initWithStore:store];
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

    auto overwrite =
        (self.settings.optionsMask & STGCalendarImportOptionsOverwrite) == STGCalendarImportOptionsOverwrite;
    strategy->import_events(events, overwrite);
}

+ (void)importToStrategyPtr:(stg::strategy *)strategyPtr
                       date:(NSDate *)date
          completionHandler:(void (^)(BOOL granted))completionHandler {
    [STGCalendarManager requestCalendarAccess:^(EKEventStore *store) {
      if (!store) {
          if (completionHandler)
              completionHandler(false);

          return;
      }

      STGCalendarImporterSettings *settings = [STGCalendarImporter defaultSettings];
      settings.date = date;

      STGCalendarImporter *calendarImporter = [[STGCalendarImporter alloc] initWithStrategyPtr:strategyPtr
                                                                                    eventStore:store
                                                                                      settings:settings];

      [calendarImporter import];

      if (completionHandler)
          completionHandler(true);
    }];
}

+ (void)saveDefaultSettings:(STGCalendarImporterSettings *)settings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;

    [userDefaults setInteger:settings.optionsMask forKey:STGCalendarImporterKeyOptions];
    [userDefaults setObject:settings.calendarsIdentifiers forKey:STGCalendarImporterKeyCalendarsIdentifiers];
}

+ (STGCalendarImporterSettings *)defaultSettings {
    NSUserDefaults *userDefaults = NSUserDefaults.standardUserDefaults;

    STGCalendarImporterSettings *settings = [[STGCalendarImporterSettings alloc] init];
    settings.optionsMask = (STGCalendarImportOptions) [userDefaults integerForKey:STGCalendarImporterKeyOptions];
    settings.calendarsIdentifiers = (NSArray<NSString *> *) [userDefaults objectForKey:STGCalendarImporterKeyCalendarsIdentifiers];

    return settings;
}

@end
