//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDARMANAGER_H
#define STRATEGR_SGCALENDARMANAGER_H

#import <AppKit/AppKit.h>
#import <EventKit/EventKit.h>

@interface SGCalendarManager : NSObject
@property(strong) EKEventStore *store;
@property(strong) NSString *calendarName;

- (instancetype)initWithStore:(EKEventStore *)store;

- (EKCalendar *)findOrCreateCalendarWithTitle:(NSString *)title
                                     andColor:(NSColor *)color;

- (void)createEventForCalendar:(EKCalendar *)calendar
                          date:(NSDate *)date
                         title:(NSString *)title
                  beginMinutes:(NSTimeInterval)beginMinutes
               durationMinutes:(NSTimeInterval)durationMinutes
          includeNotifications:(BOOL)includeNotifications;

- (void)removeAllEventsForDate:(NSDate *)date
               andCalendarName:(NSString *)calendarName;

+ (void)launchCalendarApp;
@end

#endif //STRATEGR_SGCALENDARMANAGER_H
