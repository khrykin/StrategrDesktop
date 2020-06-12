//
//  SGCalendarImportViewModel.h
//  StrategrKit
//
//  Created by Dmitry Khrykin on 10.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class SGCalendarImporterSettings;
@class EKCalendar;
@class SGCalendarImportViewModel;

typedef NS_ENUM(NSInteger, SGCheckboxState) {
    SGCheckboxStateMixed = -1,
    SGCheckboxStateOff = 0,
    SGCheckboxStateOn = 1
};

NS_SWIFT_NAME(CalendarImportViewModelDelegate)
@protocol SGCalendarImportViewModelDelegate <NSObject>

@optional
- (void)viewModel:(SGCalendarImportViewModel *)viewModel didChangeCalendars:(NSArray<EKCalendar *> *)calendars;

@end

NS_SWIFT_NAME(CalendarImportViewModel)
@interface SGCalendarImportViewModel : NSObject

@property(nonatomic, weak) id <SGCalendarImportViewModelDelegate> delegate;

@property(nonatomic, strong) NSArray<EKCalendar *> *calendars;
@property(nonatomic, readonly) SGCalendarImporterSettings *settings;
@property(nonatomic, readonly) SGCheckboxState selectedAllCheckboxState;
@property(nonatomic, readonly) BOOL isNoCalendarSelected;

- (void)didToggleCalendarAt:(NSInteger)calendarIndex;
- (void)didToggleSelectedAll;
- (void)didToggleOverwrite;
- (void)didChangeDate:(NSDate *)date NS_SWIFT_NAME(didChangeDate(_:));

- (BOOL)isCalendarSelected:(EKCalendar *)calendar;

@end

NS_SWIFT_NAME(CalendarImportViewModel.Title)
typedef NSString *SGCalendarImportViewModelTitle NS_STRING_ENUM;
static SGCalendarImportViewModelTitle const SGCalendarImportViewModelTitleOverwrite = @"Overwrite current strategy";

NS_ASSUME_NONNULL_END
