//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import <TargetConditionals.h>

#import "SGCalendarImporter.h"
#import "SGCalendarManager.h"

#ifdef TARGET_OS_MAC

#import <AppKit/AppKit.h>

#endif

#include "strategy.h"

@implementation SGCalendarImporterSettings

@end

@interface SGCalendarImporter () {
    stg::strategy *strategy;
}

@property(nonatomic, strong) EKEventStore *eventStore;
@property(nonatomic, strong) SGCalendarManager *calendarManager;

@end

@implementation SGCalendarImporter

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarImporterSettings *)settings {
    if (self = [super init]) {
        strategy = reinterpret_cast<stg::strategy *>(strategyPtr);
        self.eventStore = store;
        self.settings = settings;

        self.calendarManager = [[SGCalendarManager alloc] initWithStore:store];
    }

    return self;
}

- (void)import:(void (^)())completionHandler {
    NSArray<EKEvent *> *ekEvents = [self.calendarManager eventsWithDate:self.settings.date
                                                   calendarsIdentifiers:self.settings.calendarsIdentifiers];

    NSLog(@"events found %@", ekEvents);

    std::vector<stg::strategy::event> events;
    for (EKEvent *ekEvent in ekEvents) {
        EKCalendar *calendar = ekEvent.calendar;
        NSDate *today = [NSCalendar.currentCalendar startOfDayForDate:[[NSDate alloc] init]];
        auto beginTime = static_cast<stg::strategy::time_t>([ekEvent.startDate timeIntervalSinceDate:today] / 60);
        auto endTime = static_cast<stg::strategy::time_t>([ekEvent.endDate timeIntervalSinceDate:today] / 60);

        auto event = stg::strategy::event{ekEvent.calendar.title.UTF8String,
                                          stg::color::from_cg_color(ekEvent.calendar.color.CGColor),
                                          beginTime,
                                          endTime};

        events.push_back(event);
    }

    auto overwrite = (self.settings.optionsMask & SGCalendarImportOptionsOverwrite) == SGCalendarImportOptionsOverwrite;
    strategy->import_events(events, overwrite);

    completionHandler();
}

@end