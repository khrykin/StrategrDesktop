//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import "SGCalendarManager.h"

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

- (EKCalendar *)findOrCreateCalendarWithTitle:(NSString *)title andColor:(NSColor *)color {
    NSArray<EKCalendar *> *matchedCalendars = [self calendarsWithTitle:title];

    NSError *saveCalError = nil;

    EKCalendar *calendar;
    if ([matchedCalendars count] != 0) {
        calendar = matchedCalendars[0];

        if (![calendar.color isEqual:color]) {
            calendar.color = color;
            [self.store saveCalendar:calendar
                              commit:YES
                               error:&saveCalError];
        };

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
                  beginMinutes:(NSTimeInterval)beginMinutes
                duraionMinutes:(NSTimeInterval)durationMinutes
          includeNotifications:(BOOL)includeNotifications {
    EKEvent *event = [self makeEventForCalendar:calendar
                                           date:date
                                   beginMinutes:beginMinutes
                                durationMinutes:durationMinutes
                           includeNotifications:includeNotifications];

    unsigned currentIndex = [self.delegate calendarManagerIndexOfEvent:event] + 1;
    unsigned numberOfEvents = [self.delegate calendarManagerNumberOfEvents];

    NSError *calError = nil;

    [self.store saveEvent:event
                     span:EKSpanThisEvent
                   commit:YES
                    error:&calError];

    if (calError) {
        NSLog(@"%@", calError);
    } else {
        [self delegateProgressWithEvent:event
                           currentIndex:currentIndex
                          numberOfEvent:numberOfEvents];
    }
}

- (EKEvent *)makeEventForCalendar:(EKCalendar *)calendar
                             date:(NSDate *)date
                     beginMinutes:(NSTimeInterval)beginMinutes
                  durationMinutes:(NSTimeInterval)durationMinutes
             includeNotifications:(BOOL)includeNotifications {
    EKEvent *event = [EKEvent eventWithEventStore:self.store];
    event.title = calendar.title;

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

- (void)delegateProgressWithEvent:(EKEvent *)event
                     currentIndex:(unsigned)currentIndex
                    numberOfEvent:(unsigned)numberOfEvents {
    double progress = (float) currentIndex / (float) numberOfEvents;
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.delegate calendarManagerProgressChanged:progress];
    });
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
                             andColor:
                                     (NSColor *)color {
    EKCalendar *calendar = [EKCalendar calendarForEntityType:EKEntityTypeEvent
                                                  eventStore:self.store];

    calendar.title = title;
    calendar.color = color;

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
        [matchedSources release];
        matchedSources = [[self.store sources] filteredArrayUsingPredicate:localPredicate];
    }

    calendar.source = matchedSources[0];

    return calendar;
}

+ (void)launchCalendarApp {
    [[NSWorkspace sharedWorkspace] launchApplication:@"/Applications/Calendar.app"];
    NSArray *apps = [NSRunningApplication
            runningApplicationsWithBundleIdentifier:@"com.apple.iCal"];
    [(NSRunningApplication *) apps[0]
            activateWithOptions:NSApplicationActivateAllWindows];
};

- (void)removeAllEventsForDate:(NSDate *)date {
    NSDateComponents *offsetComponents = [[[NSDateComponents alloc] init] autorelease];
    [offsetComponents setDay:1];

    NSDate *nextDayDate
            = [[NSCalendar currentCalendar] dateByAddingComponents:offsetComponents
                                                            toDate:date
                                                           options:0];

    NSPredicate *predicate
            = [self.store predicateForEventsWithStartDate:[self beginningOfDayForDate:date]
                                                  endDate:[self beginningOfDayForDate:nextDayDate]
                                                calendars:nil];

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

- (NSArray<EKCalendar *> *)fetchCalendars {
    return [self.store calendarsForEntityType:EKEntityTypeEvent];
}

- (void)dealloc {
    [self.store release];

    [super dealloc];
}

@end