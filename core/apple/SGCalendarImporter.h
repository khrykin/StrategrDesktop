//
// Created by Dmitry Khrykin on 03/05/2020.
//

#ifndef STRATEGR_SGCALENDARIMPORTER_H
#define STRATEGR_SGCALENDARIMPORTER_H

#include <Foundation/Foundation.h>
#include <EventKit/EventKit.h>

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(CalendarImportOptions)
typedef NS_OPTIONS(unsigned, SGCalendarImportOptions) {
    SGCalendarImportOptionsOverwrite = 1 << 0
};

NS_SWIFT_NAME(CalendarImporterSettings)
@interface SGCalendarImporterSettings : NSObject

@property(nonatomic) SGCalendarImportOptions optionsMask;
@property(nonatomic, strong) NSArray<NSString *> *_Nullable calendarsIdentifiers;
@property(nonatomic, strong) NSDate *_Nullable date;

@end

NS_SWIFT_NAME(CalendarImporter)
@interface SGCalendarImporter : NSObject

+ (void)importToStrategyPtr:(void *)strategyPtr
                       date:(NSDate *)date
          completionHandler:(nullable void (^)(BOOL granted))completionHandler;

+ (void)saveDefaultSettings:(SGCalendarImporterSettings *)settings;
+ (SGCalendarImporterSettings *)defaultSettings;

@end

NS_ASSUME_NONNULL_END

#endif //STRATEGR_SGCALENDARIMPORTER_H
