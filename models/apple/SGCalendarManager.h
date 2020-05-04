//
// Created by Dmitry Khrykin on 2019-08-17.
//

#ifndef STRATEGR_SGCALENDARMANAGER_H
#define STRATEGR_SGCALENDARMANAGER_H

#import <Foundation/Foundation.h>
#import <EventKit/EventKit.h>
#import <CoreGraphics/CGColor.h>

@interface SGCalendarManager : NSObject
@property(strong) EKEventStore *store;
@property(strong) NSString *calendarName;

- (instancetype)initWithStore:(EKEventStore *)store;

- (EKCalendar *)findOrCreateCalendarWithTitle:(NSString *)title
                                     andColor:(CGColorRef)color;

- (void)createEventForCalendar:(EKCalendar *)calendar
                          date:(NSDate *)date
                         title:(NSString *)title
                  beginMinutes:(NSTimeInterval)beginMinutes
               durationMinutes:(NSTimeInterval)durationMinutes
          includeNotifications:(BOOL)includeNotifications;

- (void)removeAllEventsForDate:(NSDate *)date
               andCalendarName:(NSString *)calendarName;

- (NSArray<EKCalendar *> *)fetchCalendars;

+ (void)launchCalendarApp;

+ (void)requestCalendarAccess:(void (^)(EKEventStore *store))completionHandler;
@end

#endif //STRATEGR_SGCALENDARMANAGER_H
