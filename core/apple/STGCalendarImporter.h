//
// Created by Dmitry Khrykin on 03/05/2020.
//

#import <Foundation/Foundation.h>

#ifdef __cplusplus

#import "strategy.h"

#endif

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(CalendarImportOptions)
typedef NS_OPTIONS(NSUInteger, STGCalendarImportOptions) {
    STGCalendarImportOptionsOverwrite = 1u << 0u
};

NS_SWIFT_NAME(CalendarImporterSettings)
@interface STGCalendarImporterSettings : NSObject

@property(nonatomic) STGCalendarImportOptions optionsMask;
@property(nonatomic, strong) NSArray<NSString *> *_Nullable calendarsIdentifiers;
@property(nonatomic, strong) NSDate *_Nullable date;

@end

NS_SWIFT_NAME(CalendarImporter)
@interface STGCalendarImporter : NSObject

#ifdef __cplusplus

+ (void)importToStrategyPtr:(stg::strategy *)strategyPtr
                       date:(NSDate *)date
          completionHandler:(nullable void (^)(BOOL granted))completionHandler;

#endif

+ (void)saveDefaultSettings:(STGCalendarImporterSettings *)settings;
+ (STGCalendarImporterSettings *)defaultSettings;

@end

NS_ASSUME_NONNULL_END