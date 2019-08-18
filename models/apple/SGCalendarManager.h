//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDARMANAGER_H
#define STRATEGR_SGCALENDARMANAGER_H

#import <AppKit/AppKit.h>
#import <EventKit/EventKit.h>

@protocol SGCalendarManagerDelegate
@required
- (unsigned)calendarManagerNumberOfEvents;
- (unsigned)calendarManagerIndexOfEvent:(EKEvent *)event;
@optional
- (void)calendarManagerProgressChanged:(double)doubleValue;
@end

@interface SGCalendarManager : NSObject
@property(retain) EKEventStore *store;
@property(nonatomic, assign) id <SGCalendarManagerDelegate> delegate;

- (instancetype)initWithStore:(EKEventStore *)store;

- (NSArray<EKCalendar *> *)calendarsWithTitle:(NSString *)title;

- (EKCalendar *)findOrCreateCalendarWithTitle:(NSString *)title
                                     andColor:(NSColor *)color;

- (void)createEventForCalendar:(EKCalendar *)calendar
                          date:(NSDate *)date
                  beginMinutes:(NSTimeInterval)beginMinutes
                duraionMinutes:(NSTimeInterval)durationMinutes
          includeNotifications:(BOOL)includeNotifications;

- (void)removeAllEventsForDate:(NSDate *)date;

- (NSArray<EKCalendar *> *)fetchCalendars;

+ (void)launchCalendarApp;
@end

#endif //STRATEGR_SGCALENDARMANAGER_H
