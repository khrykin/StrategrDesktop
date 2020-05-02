//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import <TargetConditionals.h>

#import "SGCalendarManager.h"

#if !TARGET_OS_IOS

#import <AppKit/AppKit.h>

#endif

@implementation SGCalendarManager
- (instancetype)initWithStore:(EKEventStore *)store {
    if (self = [super init]) {
        self.store = store;
    }

    return self;
}

- (NSArray<EKCalendar *> *)calendarsWithTitle:(NSString *)title {
    NSPredicate *predicate = [NSPredicate predicateWithFormat:@"title==%@", title];
    return [[self fetchCalendars] filteredArrayUsingPredicate:predicate];
}

- (EKCalendar *)findOrCreateCalendarWithTitle:(NSString *)title andColor:(CGColorRef)color {
    NSArray<EKCalendar *> *matchedCalendars = [self calendarsWithTitle:title];

    NSError *saveCalError = nil;

    EKCalendar *calendar;
    if ([matchedCalendars count] != 0) {
        calendar = matchedCalendars[0];

#if !TARGET_OS_IOS
        if (color) {
            NSColor *nsColor = [NSColor colorWithCGColor:color];
            if (![calendar.color isEqual:nsColor]) {
                calendar.color = nsColor;
            }
        }
#else
        if (color && !CGColorEqualToColor(calendar.CGColor, color)) {
            calendar.CGColor = color;
        }
#endif
        [self.store saveCalendar:calendar
                          commit:YES
                           error:&saveCalError];

    } else {
        calendar = [self makeCalendarWithTitle:title andColor:color];
        [self.store saveCalendar:calendar
                          commit:YES
                           error:&saveCalError];
    }

    if (saveCalError)
        NSLog(@"%@", saveCalError);

    return calendar;
}

- (void)createEventForCalendar:(EKCalendar *)calendar
                          date:(NSDate *)date
                         title:(NSString *)title
                  beginMinutes:(NSTimeInterval)beginMinutes
               durationMinutes:(NSTimeInterval)durationMinutes
          includeNotifications:(BOOL)includeNotifications {
    EKEvent *event = [self makeEventForCalendar:calendar
                                           date:date
                                          title:title
                                   beginMinutes:beginMinutes
                                durationMinutes:durationMinutes
                           includeNotifications:includeNotifications];
    NSError *calError = nil;

    [self.store saveEvent:event
                     span:EKSpanThisEvent
                   commit:YES
                    error:&calError];

    if (calError) {
        NSLog(@"%@", calError);
    }
}

- (EKEvent *)makeEventForCalendar:(EKCalendar *)calendar
                             date:(NSDate *)date
                            title:(NSString *)title
                     beginMinutes:(NSTimeInterval)beginMinutes
                  durationMinutes:(NSTimeInterval)durationMinutes
             includeNotifications:(BOOL)includeNotifications {
    EKEvent *event = [EKEvent eventWithEventStore:self.store];
    event.title = title != nil ? title : calendar.title;

    NSDate *beginningOfToday = [self beginningOfDayForDate:date];

    event.startDate = [NSDate dateWithTimeIntervalSince1970:
            beginningOfToday.timeIntervalSince1970
            + beginMinutes * 60];
    event.endDate = [NSDate dateWithTimeIntervalSince1970:
            beginningOfToday.timeIntervalSince1970
            + (beginMinutes + durationMinutes) * 60];

    event.calendar = calendar;

    if (includeNotifications)
        event.alarms = [self makeAlarms];

    return event;
}

- (NSArray *)makeAlarms {
    EKAlarm *alarmGetReady = [EKAlarm alarmWithRelativeOffset:-5 * 60];
    EKAlarm *alarmRightNow = [EKAlarm alarmWithRelativeOffset:-30];

    return @[alarmGetReady, alarmRightNow];
}

- (NSDate *)beginningOfDayForDate:(NSDate *)date {
    NSCalendar *cal = [NSCalendar currentCalendar];
    NSDateComponents *components = [cal components:NSCalendarUnitYear |
                                                   NSCalendarUnitMonth |
                                                   NSCalendarUnitDay |
                                                   NSCalendarUnitHour |
                                                   NSCalendarUnitMinute |
                                                   NSCalendarUnitSecond
                                          fromDate:date];

    [components setHour:0];
    [components setMinute:0];
    [components setSecond:0];

    NSDate *beginningOfToday = [cal dateFromComponents:components];
    return beginningOfToday;
}

- (EKCalendar *)makeCalendarWithTitle:(NSString *)title
                             andColor:(CGColorRef)color {
    EKCalendar *calendar = [EKCalendar calendarForEntityType:EKEntityTypeEvent
                                                  eventStore:self.store];

    calendar.title = title;

#if !TARGET_OS_IOS
    calendar.color = color != nil ? [NSColor colorWithCGColor:color] : nil;
#else
    calendar.CGColor = color;
#endif

    NSPredicate *iCloudPredicate = [NSPredicate predicateWithBlock:^BOOL(EKSource *source,
                                                                         NSDictionary<NSString *, id> *bindings) {
        return source.sourceType == EKSourceTypeCalDAV &&
               [source.title isEqualToString:@"iCloud"];
    }];

    NSPredicate *localPredicate = [NSPredicate predicateWithBlock:^BOOL(EKSource *source,
                                                                        NSDictionary<NSString *, id> *bindings) {
        return source.sourceType == EKSourceTypeLocal;
    }];

    NSArray *matchedSources = [[self.store sources] filteredArrayUsingPredicate:iCloudPredicate];

    if ([matchedSources count] == 0) {
        matchedSources = [[self.store sources] filteredArrayUsingPredicate:localPredicate];
    }

    calendar.source = matchedSources[0];

    return calendar;
}

+ (void)launchCalendarApp {
#if !TARGET_OS_IOS
    [[NSWorkspace sharedWorkspace] launchApplication:@"/Applications/Calendar.app"];
    NSArray *apps = [NSRunningApplication
            runningApplicationsWithBundleIdentifier:@"com.apple.iCal"];
    [(NSRunningApplication *) apps[0]
            activateWithOptions:NSApplicationActivateAllWindows];
#endif
};

- (void)removeAllEventsForDate:(NSDate *)date
               andCalendarName:(NSString *)calendarName {
    @autoreleasepool {
        NSArray<EKCalendar *> *calendars = nil;
        if ([calendarName length] > 0)
            calendars = [self calendarsWithTitle:calendarName];

        NSDateComponents *offsetComponents = [[NSDateComponents alloc] init];
        [offsetComponents setDay:1];

        NSDate *nextDayDate
                = [[NSCalendar currentCalendar] dateByAddingComponents:offsetComponents
                                                                toDate:date
                                                               options:0];
        NSPredicate *predicate
                = [self.store predicateForEventsWithStartDate:[self beginningOfDayForDate:date]
                                                      endDate:[self beginningOfDayForDate:nextDayDate]
                                                    calendars:calendars];

        NSArray <EKEvent *> *events = [self.store eventsMatchingPredicate:predicate];

        for (EKEvent *event in events) {
            NSError *removeEventError;

            [self.store removeEvent:event
                               span:EKSpanThisEvent
                             commit:YES
                              error:&removeEventError];

            if (removeEventError) {
                NSLog(@"removeEventError: %@", removeEventError);
            }
        }
    }
}

- (NSArray<EKCalendar *> *)fetchCalendars {
    return [self.store calendarsForEntityType:EKEntityTypeEvent];
}

@end
