//
// Created by Dmitry Khrykin on 03/05/2020.
//

#ifndef STRATEGR_SGCALENDARIMPORTER_H
#define STRATEGR_SGCALENDARIMPORTER_H

#include <Foundation/Foundation.h>
#include <EventKit/EventKit.h>

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(CalendarExportOptions)
typedef NS_OPTIONS(unsigned, SGCalendarImportOptions) {
    SGCalendarImportOptionsOverwrite = 1 << 0
};

NS_SWIFT_NAME(CalendarImporterSettings)
@interface SGCalendarImporterSettings : NSObject

@property(nonatomic) SGCalendarImportOptions optionsMask;
@property(nonatomic, strong) NSArray<NSString *> *_Nullable calendarsIdentifiers;
@property(nonatomic, strong) NSDate *date;

@end

NS_SWIFT_NAME(CalendarImporter)
@interface SGCalendarImporter : NSObject

@property(nonatomic, strong) SGCalendarImporterSettings *settings;

- (instancetype)initWithStrategyPtr:(void *)strategyPtr
                         eventStore:(EKEventStore *)store
                           settings:(SGCalendarImporterSettings *)settings;

- (void)import:(void (^)())completionHandler;

@end

NS_ASSUME_NONNULL_END

#endif //STRATEGR_SGCALENDARIMPORTER_H
