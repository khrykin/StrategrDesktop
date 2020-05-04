//
// Created by Dmitry Khrykin on 03/05/2020.
//

#ifndef STRATEGR_SGCALENDARIMPORTER_H
#define STRATEGR_SGCALENDARIMPORTER_H


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
@interface SGCalendarImporterSettings : NSObject

@property(nonatomic) SGCalendarExportOptions optionsMask;
@property(nonatomic, strong) NSString *calendarName;
@property(nonatomic, strong) NSDate *date;

@end

NS_SWIFT_NAME(CalendarExporterDelegate)
@protocol SGCalendarImporterDelegate

@optional
- (void)calendarManagerProgressChanged:(double)doubleValue;

@end

NS_SWIFT_NAME(CalendarImporter)
@interface SGCalendarImporter : NSObject

typedef void (^SGCalendarImportCompletionHandler)(void *strategyPtr);

@property(nonatomic, weak) id <SGCalendarImporterDelegate> delegate;
@property(nonatomic, strong) SGCalendarImporterSettings *settings;

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarImporterSettings *)settings;

- (void)import:(SGCalendarImportCompletionHandler)completionHandler;

@end

NS_ASSUME_NONNULL_END

#endif //STRATEGR_SGCALENDARIMPORTER_H
