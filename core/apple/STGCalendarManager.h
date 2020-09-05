//
// Created by Dmitry Khrykin on 2019-08-17.
//

#import <Foundation/Foundation.h>
#import <EventKit/EventKit.h>
#import <CoreGraphics/CGColor.h>

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(CalendarManager)
@interface STGCalendarManager : NSObject

@property(strong) EKEventStore *store;

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

- (NSArray<EKCalendar *> *)calendars;

- (NSArray<EKEvent *> *)eventsWithDate:(NSDate *)date
                  calendarsIdentifiers:(NSArray<NSString *> *_Nullable)calendarsIdentifiers;

+ (void)launchCalendarAppWithDate:(NSDate *_Nullable)date;

+ (void)requestCalendarAccess:(void (^)(EKEventStore *store))completionHandler;

@end

NS_ASSUME_NONNULL_END
