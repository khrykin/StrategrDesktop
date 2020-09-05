//
//  STGCalendarImportViewModel.h
//  StrategrKit
//
//  Created by Dmitry Khrykin on 10.06.2020.
//  Copyright © 2020 Dmitry Khrykin. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class STGCalendarImporterSettings;
@class EKCalendar;
@class STGCalendarImportViewModel;

typedef NS_ENUM(NSInteger, STGCheckboxState) {
    STGCheckboxStateMixed = -1,
    STGCheckboxStateOff = 0,
    STGCheckboxStateOn = 1
};

NS_SWIFT_NAME(CalendarImportViewModelDelegate)
@protocol STGCalendarImportViewModelDelegate <NSObject>

@optional
- (void)viewModel:(STGCalendarImportViewModel *)viewModel didChangeCalendars:(NSArray<EKCalendar *> *)calendars;

@end

NS_SWIFT_NAME(CalendarImportViewModel)
@interface STGCalendarImportViewModel : NSObject

@property(nonatomic, weak) id <STGCalendarImportViewModelDelegate> delegate;

@property(nonatomic, strong) NSArray<EKCalendar *> *calendars;
@property(nonatomic, readonly) STGCalendarImporterSettings *settings;
@property(nonatomic, readonly) STGCheckboxState selectedAllCheckboxState;
@property(nonatomic, readonly) BOOL isNoCalendarSelected;

- (void)didToggleCalendarAt:(NSInteger)calendarIndex;
- (void)didToggleSelectedAll;
- (void)didToggleOverwrite;
- (void)didChangeDate:(NSDate *)date NS_SWIFT_NAME(didChangeDate(_:));

- (BOOL)isCalendarSelected:(EKCalendar *)calendar;

@end

NS_SWIFT_NAME(CalendarImportViewModel.Title)
typedef NSString *STGCalendarImportViewModelTitle NS_STRING_ENUM;
static STGCalendarImportViewModelTitle const STGCalendarImportViewModelTitleOverwrite = @"Overwrite current strategy";

NS_ASSUME_NONNULL_END
