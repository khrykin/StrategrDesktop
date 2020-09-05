//
// Created by Dmitry Khrykin on 2020-02-10.
//

#import <Foundation/Foundation.h>
#import <EventKit/EventKit.h>

#ifdef __cplusplus

#import "strategy.h"

#endif

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(CalendarExportOptions)
typedef NS_OPTIONS(NSUInteger, STGCalendarExportOptions) {
    STGCalendarExportOptionsOverwrite = 1u << 0u,
    STGCalendarExportOptionsIncludeNotifications = 1u << 1u,
    STGCalendarExportOptionsUseSpecificCalendar = 1u << 2u
};

NS_SWIFT_NAME(CalendarExporterSettings)
@interface STGCalendarExporterSettings : NSObject

@property(nonatomic) STGCalendarExportOptions optionsMask;
@property(nonatomic, strong) NSString *_Nullable calendarName;
@property(nonatomic, strong) NSDate *_Nullable date;

@end

NS_SWIFT_NAME(CalendarExporterDelegate)
@protocol STGCalendarExporterDelegate

@optional
- (void)calendarExporterProgressChanged:(double)doubleValue;

@end

NS_SWIFT_NAME(CalendarExporter)
@interface STGCalendarExporter : NSObject

@property(nonatomic, weak) id <STGCalendarExporterDelegate> delegate;
@property(nonatomic, readonly) NSDate *date;

#ifdef __cplusplus

+ (void)exportFromStrategyPtr:(const stg::strategy *)strategy
                         date:(NSDate *_Nullable)date
                     delegate:(nullable id <STGCalendarExporterDelegate>)delegate
            completionHandler:(nullable void (^)(BOOL exported))completionHandler;

#endif

+ (void)saveDefaultSettings:(STGCalendarExporterSettings *)settings;
+ (STGCalendarExporterSettings *)defaultSettings;

@end

NS_ASSUME_NONNULL_END
