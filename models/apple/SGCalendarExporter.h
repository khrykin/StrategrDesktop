//
// Created by Dmitry Khrykin on 2020-02-10.
//

#ifndef STRATEGR_SGCALENDAREXPORTER_H
#define STRATEGR_SGCALENDAREXPORTER_H

#include <Foundation/Foundation.h>
#include <EventKit/EventKit.h>

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(CalendarExportOptions)
typedef NS_OPTIONS(unsigned, SGCalendarExportOptions) {
    SGCalendarExportOptionsOverwriteIsOn = 1 << 0,
    SGCalendarExportOptionsNotificationsIsOn = 1 << 1,
    SGCalendarExportOptionsSpecificCalendarIsOn = 1 << 2
};

NS_SWIFT_NAME(CalendarExporterSettings)
@interface SGCalendarExporterSettings : NSObject

@property(nonatomic) SGCalendarExportOptions optionsMask;
@property(nonatomic, strong) NSString *calendarName;
@property(nonatomic, strong) NSDate *date;

@end

NS_SWIFT_NAME(CalendarExporterDelegate)
@protocol SGCalendarExporterDelegate

@optional
- (void)calendarManagerProgressChanged:(double)doubleValue;

@end

NS_SWIFT_NAME(CalendarExporter)
@interface SGCalendarExporter : NSObject

@property(nonatomic, weak) id <SGCalendarExporterDelegate> delegate;
@property(nonatomic, strong) SGCalendarExporterSettings *settings;

+ (void)requestExport:(void (^)(EKEventStore *store))completionHandler;

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarExporterSettings *)settings;

- (void)export:(void (^)())completionHandler;

@end

NS_ASSUME_NONNULL_END

#endif //STRATEGR_SGCALENDAREXPORTER_H
